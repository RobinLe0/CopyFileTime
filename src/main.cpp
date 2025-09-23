#include <cstdio>
#include <filesystem>

#include "Options.hpp"
#include "CopyTimes.hpp"



int wmain(int argc, wchar_t* argv[])
{
    namespace fs = std::filesystem;

    Options opt(argc, argv);

    if (!opt.ok())
    {
        std::fprintf(stderr, "%s\n", opt.errorMessage().c_str());
        return 1;
    }



    // single file
    if (!opt.isDir())
    {
        if (!CopyFileTimes(opt.sourcePath(), opt.destPath()))
            return 1;
    }
    
    // directory
    else
    {
        const auto sSourceRoot = fs::path(opt.sourcePath()).relative_path().u8string();
        const auto sDestRoot   = fs::path(opt.destPath  ()).u8string();

        uint64_t iUnmatchedCount = 0;
        uint64_t iSucceededCount = 0;
        uint64_t iFailedCount    = 0;

        for (const auto &item : fs::recursive_directory_iterator(opt.sourcePath()))
        {
            const fs::path pathDest =
                sDestRoot +
                u8'\\' +
                item.path().relative_path().u8string().substr(sSourceRoot.length());

            if (item.is_regular_file())
            {
                if (!fs::is_regular_file(pathDest))
                {
                    ++iUnmatchedCount;
                    continue;
                }

                if (!CopyFileTimes(item.path().wstring(), pathDest.wstring()))
                    ++iFailedCount;
                else
                    ++iSucceededCount;
            }
            else if (item.is_directory())
            {
                if (!fs::is_directory(pathDest))
                {
                    ++iUnmatchedCount;
                    continue;
                }

                if (!CopyDirTimes(item.path().wstring(), pathDest.wstring()))
                    ++iFailedCount;
                else
                    ++iSucceededCount;
            }
        }

        std::printf(
            "\n%llu unmatched, %llu succeeded, %llu failed.\n",
            iUnmatchedCount,
            iSucceededCount,
            iFailedCount);
    }



    return 0;
}

