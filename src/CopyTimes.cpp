#include "CopyTimes.hpp"

#include "ScopedHandle.hpp"

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <filesystem>



namespace
{

    std::string PathToStr(const std::wstring &sPath)
    {
        return std::filesystem::path(sPath).string();
    }

    bool CopyTimes(const std::wstring &sSource, const std::wstring &sDest, DWORD dwFlagsAndAttributes)
    {
        struct FileTimes
        {
            FILETIME oCreationTime;
            FILETIME oLastAccessTime;
            FILETIME oLastWriteTime;
        } times;

        ScopedHandle hFile = CreateFileW(
                sSource.c_str(),      // lpFileName
                GENERIC_READ,         // dwDesiredAccess
                FILE_SHARE_READ,      // dwShareMode
                NULL,                 // lpSecurityAttributes
                OPEN_EXISTING,        // dwCreationDisposition
                dwFlagsAndAttributes, // dwFlagsAndAttributes
                NULL                  // hTemplateFile
        );
        if (hFile.get() == INVALID_HANDLE_VALUE)
        {
            std::fprintf(
                stderr,
                "Failed to open \"%s\" for reading (error %d).",
                PathToStr(sSource).c_str(),
                GetLastError()
            );
            return false;
        }
        if (!GetFileTime(
            hFile,                  // hFile
            &times.oCreationTime,   // lpCreationTime
            &times.oLastAccessTime, // lpLastAccessTime
            &times.oLastWriteTime   // lpLastWriteTime
        ))
        {
            std::fprintf(
                stderr,
                "GetFileTime failed on \"%s\" (error %d).\n",
                PathToStr(sSource).c_str(),
                GetLastError()
            );
            return false;
        }

        hFile.close();
        hFile = CreateFileW(
            sDest.c_str(),        // lpFileName
            GENERIC_WRITE,        // dwDesiredAccess
            NULL,                 // dwShareMode
            NULL,                 // lpSecurityAttributes
            OPEN_EXISTING,        // dwCreationDisposition
            dwFlagsAndAttributes, // dwFlagsAndAttributes
            NULL                  // hTemplateFile
        );
        if (hFile.get() == INVALID_HANDLE_VALUE)
        {
            std::fprintf(
                stderr,
                "Failed to open \"%s\" for writing (error %d).",
                PathToStr(sSource).c_str(),
                GetLastError()
            );
            return false;
        }
        if (!SetFileTime(
            hFile,                  // hFile
            &times.oCreationTime,   // lpCreationTime
            &times.oLastAccessTime, // lpLastAccessTime
            &times.oLastWriteTime   // lpLastWriteTime
        ))
        {
            std::fprintf(
                stderr,
                "SetFileTime failed on \"%s\" (error %d).\n",
                PathToStr(sDest).c_str(),
                GetLastError()
            );
            return false;
        }

        return true;
    }
}

bool CopyFileTimes(const std::wstring &sSource, const std::wstring &sDest)
{
    return CopyTimes(sSource, sDest, 0);
}

bool CopyDirTimes(const std::wstring &sSource, const std::wstring &sDest)
{
    return CopyTimes(sSource, sDest, FILE_FLAG_BACKUP_SEMANTICS);
}
