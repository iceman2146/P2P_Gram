
#include "wx/wxprec.h"

// for all others, include the necessary headers
#ifndef WX_PRECOMP
#  include "wx/wx.h"
#endif

#include "wx/socket.h"

#include "wx/sstream.h"
#include "wx/thread.h"
#include "wx/scopedptr.h"
#include "logger.h"

// --------------------------------------------------------------------------
// resources
// --------------------------------------------------------------------------

// the application icon
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.ico"
#endif

// --------------------------------------------------------------------------
// classes
// --------------------------------------------------------------------------

// Define a new application type
class ClientApp : public wxApp
{
public:
  virtual bool OnInit() wxOVERRIDE;
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
  MyFrame();
  ~MyFrame();

  // event handlers for File menu
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);

  // event handlers for Socket menu
  void OnOpenConnection(wxCommandEvent& event);
  void OnTest1(wxCommandEvent& event);
  void OnTest2(wxCommandEvent& event);
  void OnTest3(wxCommandEvent& event);
  void OnCloseConnection(wxCommandEvent& event);


#if wxUSE_IPV6
  void OnOpenConnectionIPv6(wxCommandEvent& event);
#endif

  void OpenConnection(wxSockAddress::Family family);

  // event handlers for DatagramSocket menu (stub)
  void OnDatagram(wxCommandEvent& event);

  // socket event handler
  void OnSocketEvent(wxSocketEvent& event);

  // convenience functions
  void UpdateStatusBar();

private:
  wxSocketClient *m_sock;
  wxTextCtrl     *m_text;
  wxMenu         *m_menuFile;
  wxMenu         *m_menuSocket;
  wxMenu         *m_menuDatagramSocket;
  wxMenu         *m_menuProtocols;
  wxMenuBar      *m_menuBar;
  bool            m_busy;

  // any class wishing to process wxWidgets events must use this macro
  wxDECLARE_EVENT_TABLE();
};


// IDs for the controls and the menu commands
enum
{
  // menu items
  CLIENT_QUIT = wxID_EXIT,
  CLIENT_ABOUT = wxID_ABOUT,
  CLIENT_OPEN = 100,
#if wxUSE_IPV6
  CLIENT_OPENIPV6,
#endif
  CLIENT_TEST1,
  CLIENT_TEST2,
  CLIENT_TEST3,
  CLIENT_CLOSE,

  CLIENT_DGRAM,

  // id for socket
  SOCKET_ID
};



// --------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// --------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(CLIENT_QUIT,     MyFrame::OnQuit)
  EVT_MENU(CLIENT_ABOUT,    MyFrame::OnAbout)
  EVT_MENU(CLIENT_OPEN,     MyFrame::OnOpenConnection)
#if wxUSE_IPV6
  EVT_MENU(CLIENT_OPENIPV6, MyFrame::OnOpenConnectionIPv6)
#endif
  EVT_MENU(CLIENT_TEST1,    MyFrame::OnTest1)
  EVT_MENU(CLIENT_TEST2,    MyFrame::OnTest2)
  EVT_MENU(CLIENT_TEST3,    MyFrame::OnTest3)
  EVT_MENU(CLIENT_CLOSE,    MyFrame::OnCloseConnection)
  EVT_MENU(CLIENT_DGRAM,    MyFrame::OnDatagram)

  EVT_SOCKET(SOCKET_ID,     MyFrame::OnSocketEvent)
wxEND_EVENT_TABLE()




wxIMPLEMENT_APP(ClientApp);
