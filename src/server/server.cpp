#include "server.h"

bool ServerApp::OnInit()
{
  FreeConsole();
  
  if (!wxApp::OnInit())
    return false;

  server_main_Frame *frame = new server_main_Frame();
  frame->Show(true);
  return true;
}

server_main_Frame::server_main_Frame() : wxFrame((wxFrame *)NULL, wxID_ANY,
                                                 _("Сервер"),
                                                 wxDefaultPosition, wxSize(640, 480))
{
  // Give the frame an icon
  SetIcon(wxICON(sample));

  // Make menus
  m_menuFile = new wxMenu();
  m_menuFile->Append(SERVER_WAITFORACCEPT, "&Ждать соеденения\tCtrl-W");
  m_menuFile->Append(SERVER_UDPTEST, "&UDP тест\tCtrl-U");
  m_menuFile->AppendSeparator();
  m_menuFile->Append(SERVER_ABOUT, _("&Справка\tCtrl-A"), _("Показать справку"));
  m_menuFile->AppendSeparator();
  m_menuFile->Append(SERVER_QUIT, _("Выйти\tAlt-X"), _("Закрыть сервер"));
  // Append menus to the menubar
  m_menuBar = new wxMenuBar();
  m_menuBar->Append(m_menuFile, _("&Файл"));
  SetMenuBar(m_menuBar);
  // Status bar
  CreateStatusBar(2);
  // Make a textctrl for logging
  m_text = new wxTextCtrl(this, wxID_ANY,
                          _("программа запускается: Сервер\n"),
                          wxDefaultPosition, wxDefaultSize,
                          wxTE_MULTILINE | wxTE_READONLY);
  delete wxLog::SetActiveTarget(new wxLogTextCtrl(m_text));

  // Create the address - defaults to localhost:0 initially
  IPaddress addr;
  addr.Service(3000);

  wxLogMessage("создание соединения по адресу %s порт :%u", addr.IPAddress(), addr.Service());

  // Create the socket
  m_server = new wxSocketServer(addr);

  // We use IsOk() here to see if the server is really listening
  if (!m_server->IsOk())
  {
    wxLogMessage("Порт занят");
    return;
  }

  IPaddress addrReal;
  if (!m_server->GetLocal(addrReal))
  {
    wxLogMessage("ERROR: couldn't get the address we bound to");
  }
  else
  {
    wxLogMessage("сервер слушает в %s порт :%u",
                 addrReal.IPAddress(), addrReal.Service());
  }

  // Setup the event handler and subscribe to connection events
  m_server->SetEventHandler(*this, SERVER_ID);
  m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
  m_server->Notify(true);

  m_busy = false;
  m_numClients = 0;
  UpdateStatusBar();
}

server_main_Frame::~server_main_Frame()
{
  // No delayed deletion here, as the frame is dying anyway
  delete m_server;
}

// event handlers

void server_main_Frame::OnQuit(wxCommandEvent &WXUNUSED(event))
{
  // true is to force the frame to close
  Close(true);
}

void server_main_Frame::OnAbout(wxCommandEvent &WXUNUSED(event))
{
  wxMessageBox(_("Сервер\nИтоговая проектная работа, курса Otus\n"),
               _("Справка"),
               wxOK | wxICON_INFORMATION, this);
}

void server_main_Frame::OnUDPTest(wxCommandEvent &WXUNUSED(event))
{
  TestLogger logtest("UDP test");

  IPaddress addr;
  addr.Service(3000);
  wxDatagramSocket sock(addr);

  char buf[1024];
  size_t n = sock.RecvFrom(addr, buf, sizeof(buf)).LastCount();
  if (!n)
  {
    wxLogMessage("ERROR: failed to receive data");
    return;
  }

  wxLogMessage("Received \"%s\" from %s:%u.",
               wxString::From8BitData(buf, n),
               addr.IPAddress(), addr.Service());

  for (size_t i = 0; i < n; i++)
  {
    char &c = buf[i];
    if ((c >= 'A' && c <= 'M') || (c >= 'a' && c <= 'm'))
      c += 13;
    else if ((c >= 'N' && c <= 'Z') || (c >= 'n' && c <= 'z'))
      c -= 13;
  }

  if (sock.SendTo(addr, buf, n).LastCount() != n)
  {
    wxLogMessage("ERROR: failed to send data");
    return;
  }
}

void server_main_Frame::OnWaitForAccept(wxCommandEvent &WXUNUSED(event))
{
  TestLogger logtest("WaitForAccept() test");

  wxBusyInfo info("Waiting for connection for 10 seconds...", this);
  if (m_server->WaitForAccept(10))
    wxLogMessage("Accepted client connection.");
  else
    wxLogMessage("Connection error or timeout expired.");
}

void server_main_Frame::Test1(wxSocketBase *sock)
{
  TestLogger logtest("Test 1");

  // Receive data from socket and send it back. We will first
  // get a byte with the buffer size, so we can specify the
  // exact size and use the wxSOCKET_WAITALL flag. Also, we
  // disabled input events so we won't have unwanted reentrance.
  // This way we can avoid the infamous wxSOCKET_BLOCK flag.

  sock->SetFlags(wxSOCKET_WAITALL);

  // Read the size
  unsigned char len;
  sock->Read(&len, 1);
  wxCharBuffer buf(len);

  // Read the data
  sock->Read(buf.data(), len);
  wxLogMessage("Got the data, sending it back");

  // Write it back
  sock->Write(buf, len);
}

void server_main_Frame::Test2(wxSocketBase *sock)
{
  char buf[4096];

  TestLogger logtest("Test 2");

  // We don't need to set flags because ReadMsg and WriteMsg
  // are not affected by them anyway.

  // Read the message
  wxUint32 len = sock->ReadMsg(buf, sizeof(buf)).LastCount();
  if (!len)
  {
    wxLogError("Failed to read message.");
    return;
  }


  wxLogMessage("Got from \"%s\" .",wxString::FromUTF8(buf, len));
  wxLogMessage("Sending the data back");

  // Write it back
  sock->WriteMsg(buf, len);
}

void server_main_Frame::Test3(wxSocketBase *sock)
{
  TestLogger logtest("Test 3");

  // This test is similar to the first one, but the len is
  // expressed in kbytes - this tests large data transfers.

  sock->SetFlags(wxSOCKET_WAITALL);

  // Read the size
  unsigned char len;
  sock->Read(&len, 1);
  wxCharBuffer buf(len * 1024);

  // Read the data
  sock->Read(buf.data(), len * 1024);
  wxLogMessage("Got the data, sending it back");

  // Write it back
  sock->Write(buf, len * 1024);
}

void server_main_Frame::OnServerEvent(wxSocketEvent &event)
{
  wxSocketBase *sock;
  // Accept new connection if there is one in the pending
  // connections queue, else exit. We use Accept(false) for
  // non-blocking accept (although if we got here, there
  // should ALWAYS be a pending connection).
  sock = m_server->Accept(false);
  if (sock)
  {
    IPaddress addr;
    if (!sock->GetPeer(addr))
    {
      wxLogMessage("New connection from unknown client accepted.");
    }
    else
    {
      wxLogMessage("New client connection from %s:%u accepted",
                   addr.IPAddress(), addr.Service());
    }
  }
  else
  {
    wxLogMessage("Error: couldn't accept a new connection");
    return;
  }
  
  
  
  sock->SetEventHandler(*this, SOCKET_ID);
  sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
  sock->Notify(true);
  /*
   char buf[4096];
  wxUint32 len = sock->ReadMsg(buf, sizeof(buf)).LastCount();
    client_data* client = new client_data;
  client->client_id = m_numClients;
 client->client_name =wxString::FromUTF8(buf, len);
 */

  m_numClients++;



// m_text->AppendText(client->client_name);
  UpdateStatusBar();
}

void server_main_Frame::OnSocketEvent(wxSocketEvent &event)
{
  wxString s = _("OnSocketEvent: ");
  wxSocketBase *sock = event.GetSocket();

  // First, print a message
  switch (event.GetSocketEvent())
  {
  case wxSOCKET_INPUT:
    s.Append(_("wxSOCKET_INPUT\n"));
    break;
  case wxSOCKET_LOST:
    s.Append(_("wxSOCKET_LOST\n"));
    break;
  default:
    s.Append(_("Unexpected event !\n"));
    break;
  }

  m_text->AppendText(s);

  // Now we process the event
  switch (event.GetSocketEvent())
  {
  case wxSOCKET_INPUT:
  {
    // We disable input events, so that the test doesn't trigger
    // wxSocketEvent again.
    sock->SetNotify(wxSOCKET_LOST_FLAG);

    // Which test are we going to run?
    unsigned char c;
    sock->Read(&c, 1);

    switch (c)
    {
    case 0xBE:
      Test1(sock);
      break;
    case 0xCE:
      Test2(sock);
      break;
    case 0xDE:
      Test3(sock);
      break;
    default:
      wxLogMessage("Unknown test id received from client");
    }

    // Enable input events again.
    sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
    break;
  }
  case wxSOCKET_LOST:
  {
    m_numClients--;

    // Destroy() should be used instead of delete wherever possible,
    // due to the fact that wxSocket uses 'delayed events' (see the
    // documentation for wxPostEvent) and we don't want an event to
    // arrive to the event handler (the frame, here) after the socket
    // has been deleted. Also, we might be doing some other thing with
    // the socket at the same time; for example, we might be in the
    // middle of a test or something. Destroy() takes care of all
    // this for us.

    wxLogMessage("Deleting socket.");
    sock->Destroy();
    break;
  }
  default:;
  }

  UpdateStatusBar();
}

// convenience functions

void server_main_Frame::UpdateStatusBar()
{
#if wxUSE_STATUSBAR
  wxString s;
  s.Printf(_("%d clients connected"), m_numClients);
  SetStatusText(s, 1);
#endif // wxUSE_STATUSBAR
}
