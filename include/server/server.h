
#include "wx/wxprec.h"

// for all others, include the necessary headers
#ifndef WX_PRECOMP
#  include "wx/wx.h"
#endif

#include "wx/busyinfo.h"
#include "wx/socket.h"
#include "logger.h"

// this example is currently written to use only IP or only IPv6 sockets, it
// should be extended to allow using either in the future
#if wxUSE_IPV6
    typedef wxIPV6address IPaddress;
#else
    typedef wxIPV4address IPaddress;
#endif

// --------------------------------------------------------------------------
// resources
// --------------------------------------------------------------------------

// the application icon
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// --------------------------------------------------------------------------
// classes
// --------------------------------------------------------------------------

// Define a new application type
class ServerApp : public wxApp
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

  // event handlers (these functions should _not_ be virtual)
  void OnUDPTest(wxCommandEvent& event);
  void OnWaitForAccept(wxCommandEvent& event);
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnServerEvent(wxSocketEvent& event);
  void OnSocketEvent(wxSocketEvent& event);

  void Test1(wxSocketBase *sock);
  void Test2(wxSocketBase *sock);
  void Test3(wxSocketBase *sock);

  // convenience functions
  void UpdateStatusBar();

private:
  wxSocketServer *m_server;
  wxTextCtrl     *m_text;
  wxMenu         *m_menuFile;
  wxMenuBar      *m_menuBar;
  bool            m_busy;
  int             m_numClients;

  // any class wishing to process wxWidgets events must use this macro
  wxDECLARE_EVENT_TABLE();
};
// --------------------------------------------------------------------------
// constants
// --------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
  // menu items
  SERVER_UDPTEST = 10,
  SERVER_WAITFORACCEPT,
  SERVER_QUIT = wxID_EXIT,
  SERVER_ABOUT = wxID_ABOUT,

  // id for sockets
  SERVER_ID = 100,
  SOCKET_ID
};

// --------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// --------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(SERVER_QUIT,  MyFrame::OnQuit)
  EVT_MENU(SERVER_ABOUT, MyFrame::OnAbout)
  EVT_MENU(SERVER_UDPTEST, MyFrame::OnUDPTest)
  EVT_MENU(SERVER_WAITFORACCEPT, MyFrame::OnWaitForAccept)
  EVT_SOCKET(SERVER_ID,  MyFrame::OnServerEvent)
  EVT_SOCKET(SOCKET_ID,  MyFrame::OnSocketEvent)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(ServerApp);