#include "Options.hpp"

#include <filesystem>
#include <format>



Options::Options(int iArgC, wchar_t *szArgV[])
{
    namespace fs = std::filesystem;

    if (iArgC != 3)
    {
        m_sErrorMessage = std::format("Expected 2 arguments but found {}.", iArgC - 1);
        return;
    }

    m_sSourcePath = szArgV[1];
    m_sDestPath   = szArgV[2];

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
                "Source path \"{}\" is a file, but destionation path \"{}\" is not.",
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
                "Source path \"{}\" is a directory, but destionation path \"{}\" is not.",
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
}
