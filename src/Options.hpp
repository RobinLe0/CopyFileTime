#ifndef COPYFILETIME_OPTIONS
#define COPYFILETIME_OPTIONS





#include <string>
#include <string_view>



namespace Parameters
{

    constexpr char szAPPLICATION_NAME[] = "CopyFileTime";

    constexpr wchar_t szOPTION_HELP[]         = L"--help";
    constexpr wchar_t szOPTION_NOPERCENTAGE[] = L"--no-percentage";
    constexpr wchar_t szOPTION_VERBOSE[]      = L"--verbose";

    constexpr wchar_t cFLAG_HELP         = L'h';
    constexpr wchar_t cFLAG_NOPERCENTAGE = L'n';
    constexpr wchar_t cFLAG_VERBOSE      = L'v';

}



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

    bool help() const { return m_bHelp; }
    bool verbose() const { return m_bVerbose; }
    bool showPercentage() const { return m_bShowPercentage; }


private: // methods

    bool ProcessOption(const std::wstring_view &sv);
    bool ProcessOptionFlag(wchar_t c);


private: // variables

    std::string m_sErrorMessage;

    bool m_bIsDir = false;

    std::wstring m_sSourcePath;
    std::wstring m_sDestPath;

    bool m_bHelp           = false;
    bool m_bVerbose        = false;
    bool m_bShowPercentage = true;

};





#endif // COPYFILETIME_OPTIONS