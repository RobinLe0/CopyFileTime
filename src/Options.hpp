#ifndef COPYFILETIME_OPTIONS
#define COPYFILETIME_OPTIONS





#include <string>



class Options final
{
public: // methods

    Options(int iArgC, wchar_t *szArgV[]);
    ~Options() = default;

    bool ok() const { return m_sErrorMessage.empty(); }

    const std::string &errorMessage() const { return m_sErrorMessage; }

    const std::wstring &sourcePath() const { return m_sSourcePath; }
    const std::wstring &destPath() const { return m_sDestPath; }

    bool isDir() const { return m_bIsDir; }


private: // variables

    std::string m_sErrorMessage;

    bool m_bIsDir = false;

    std::wstring m_sSourcePath;
    std::wstring m_sDestPath;

};





#endif // COPYFILETIME_OPTIONS