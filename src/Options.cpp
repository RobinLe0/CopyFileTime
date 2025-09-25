#include "Options.hpp"

#include <filesystem>
#include <format>



Options::Options(int iArgC, wchar_t *szArgV[])
{
    namespace fs = std::filesystem;

    if (iArgC < 3)
    {
        if (iArgC == 2)
        {
            std::wstring_view svArg = szArgV[1];
            if (svArg == Parameters::szOPTION_HELP ||
                (svArg.length() == 2 && svArg[0] == L'-' && svArg[1] == Parameters::cFLAG_HELP)
            )
            {
                m_bHelp = true;
                return;
            }
        }

        m_sErrorMessage = std::format("Expected at least 2 parameters but found {}.", iArgC - 1);
        m_bHelp         = true;
        return;
    }

    m_sSourcePath = szArgV[1];
    m_sDestPath   = szArgV[2];



    // validate the given paths

    auto pathSource = fs::path(m_sSourcePath);
    auto pathDest   = fs::path(m_sDestPath);

    if (!fs::exists(pathSource))
    {
        m_sErrorMessage = std::format("Source path \"{}\" doesn't exist.", pathSource.string());
        return;
    }
    else if (fs::is_regular_file(pathSource))
    {
        m_bIsDir = false;

        if (!fs::is_regular_file(pathDest))
        {
            m_sErrorMessage = std::format(
                "Source path \"{}\" is a file, but destination path \"{}\" is not.",
                pathSource.string(),
                pathDest  .string()
            );
            return;
        }
    }
    else if (fs::is_directory(pathSource))
    {
        m_bIsDir = true;

        if (!fs::is_directory(pathDest))
        {
            m_sErrorMessage = std::format(
                "Source path \"{}\" is a directory, but destination path \"{}\" is not.",
                pathSource.string(),
                pathDest.string()
            );
            return;
        }
    }
    else
    {
        m_sErrorMessage = std::format(
            "Source path \"{}\" is neither a file nor a directory.",
            pathSource.string()
        );
        return;
    }
    
    if ((pathSource = fs::absolute(pathSource)) == (pathDest = fs::absolute(pathDest)))
    {
        m_sErrorMessage = "Source and destination are identical.";
        return;
    }



    // read through the remaining parameters

    for (int iArg = 3; iArg < iArgC; ++iArg)
    {
        const std::wstring_view svArg = szArgV[iArg];

        if (svArg.starts_with(L'-'))
        {
            if (svArg.starts_with(L"--"))
            {
                if (!ProcessOption(svArg))
                {
                    bool bASCII = true;
                    std::string sNarrowArg;
                    sNarrowArg.reserve(svArg.length());

                    for (size_t i = 0; i < svArg.length(); ++i)
                    {
                        const auto c = svArg[i];

                        if (c >= 0x100)
                        {
                            bASCII = false;
                            break;
                        }
                        else
                            sNarrowArg += (char)c;
                    }

                    if (bASCII)
                        m_sErrorMessage = std::format("Unknown option \"{}\".", sNarrowArg);
                    else
                        m_sErrorMessage = std::format("Unknown option at position {}.", iArg);
                    return;
                }
            }
            else
            {
                for (auto c : svArg.substr(1))
                {
                    if (!ProcessOptionFlag(c))
                    {
                        if (c < 0x100)
                            m_sErrorMessage = std::format("Unknown flag \"{}\".", (char)c);
                        else
                            m_sErrorMessage = std::format("Unknown non-ASCII flag in parameter {}.",
                                                          iArg
                                              );
                        return;
                    }
                }
            }
        }
    }

}

bool Options::ProcessOption(const std::wstring_view &sv)
{
    if (sv == Parameters::szOPTION_HELP)
        m_bHelp = true;
    else if (sv == Parameters::szOPTION_VERBOSE)
        m_bVerbose = true;
    else if (sv == Parameters::szOPTION_NOPERCENTAGE)
        m_bShowPercentage = false;
    else
        return false;

    return true;
}

bool Options::ProcessOptionFlag(wchar_t c)
{
    switch (c)
    {
    case Parameters::cFLAG_HELP:
        m_bHelp = true;
        break;

    case Parameters::cFLAG_VERBOSE:
        m_bVerbose = true;
        break;

    case Parameters::cFLAG_NOPERCENTAGE:
        m_bShowPercentage = false;
        break;

    default:
        return false;
    }

    return true;
}
