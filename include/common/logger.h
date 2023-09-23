#include "wx/wxprec.h"

// for all others, include the necessary headers
#ifndef WX_PRECOMP
#  include "wx/wx.h"
#endif

// simple helper class to log start and end of each test

class TestLogger
{
public:
    TestLogger(const wxString& name)    : m_name(name)
    {
        wxLogMessage("=== %s begins ===", m_name);
    }
    ~TestLogger()
    { wxLogMessage("=== %s ends ===", m_name);}
private:
    const wxString m_name;
};

