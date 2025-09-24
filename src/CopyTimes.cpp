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
                FILE_READ_ATTRIBUTES, // dwDesiredAccess
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
                "\nFailed to open \"%s\" for reading (error %d).\n",
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
                "\nGetFileTime failed on \"%s\" (error %d).\n",
                PathToStr(sSource).c_str(),
                GetLastError()
            );
            return false;
        }

        hFile.close();
        hFile = CreateFileW(
            sDest.c_str(),         // lpFileName
            FILE_WRITE_ATTRIBUTES, // dwDesiredAccess
            FILE_SHARE_READ,       // dwShareMode
            NULL,                  // lpSecurityAttributes
            OPEN_EXISTING,         // dwCreationDisposition
            dwFlagsAndAttributes,  // dwFlagsAndAttributes
            NULL                   // hTemplateFile
        );
        if (hFile.get() == INVALID_HANDLE_VALUE)
        {
            std::fprintf(
                stderr,
                "\nFailed to open \"%s\" for writing (error %d).\n",
                PathToStr(sDest).c_str(),
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
                "\nSetFileTime failed on \"%s\" (error %d).\n",
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
    return CopyTimes(sSource, sDest, FILE_ATTRIBUTE_NORMAL);
}

bool CopyDirTimes(const std::wstring &sSource, const std::wstring &sDest)
{
    return CopyTimes(sSource, sDest, FILE_FLAG_BACKUP_SEMANTICS);
}
