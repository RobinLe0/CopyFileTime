#ifndef COPYFILETIME_SCOPEDHANDLE
#define COPYFILETIME_SCOPEDHANDLE





#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>



class ScopedHandle final
{
public: // methods

    ScopedHandle(HANDLE h) : m_h(h) {}
    ~ScopedHandle() { close(); }

    void close()
    {
        CloseHandle(m_h);
        m_h = NULL;
    }

    HANDLE get() const { return m_h; }

    operator HANDLE() const { return m_h; }


private: // variables

    HANDLE m_h;

};





#endif // COPYFILETIME_SCOPEDHANDLE