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

        std::printf("Counting files... ");
        uint64_t iTotalCount = 0;
        for (const auto &item : fs::recursive_directory_iterator(opt.sourcePath()))
        {
            if (item.is_regular_file() || item.is_directory())
                ++iTotalCount;
        }
        std::printf("Found %llu.\n", iTotalCount);
        if (iTotalCount == 0)
        {
            std::printf("Exiting.\n");
            return 0;
        }


        std::string sProgress =
            "[                                 00 %                                 ]";
        constexpr unsigned iMAX_PROGRESS      = 70;
        constexpr unsigned iPERCENTAGE_OFFSET = 34;

        unsigned iPrevPercent  = 0;
        unsigned iPrevProgress = 0;

        std::printf("Copying times...\n");
        std::printf("%s", sProgress.c_str());
        uint64_t iCurrentIdx = 0;
        for (const auto &item : fs::recursive_directory_iterator(opt.sourcePath()))
        {
            const fs::path pathDest =
                sDestRoot +
                u8'\\' +
                item.path().relative_path().u8string().substr(sSourceRoot.length());

            if (item.is_regular_file())
            {
                if (!fs::is_regular_file(pathDest))
                    ++iUnmatchedCount;
                else
                {
                    if (!CopyFileTimes(item.path().wstring(), pathDest.wstring()))
                        ++iFailedCount;
                    else
                        ++iSucceededCount;
                }
            }
            else if (item.is_directory())
            {
                if (!fs::is_directory(pathDest))
                    ++iUnmatchedCount;
                else
                {
                    if (!CopyDirTimes(item.path().wstring(), pathDest.wstring()))
                        ++iFailedCount;
                    else
                        ++iSucceededCount;
                }
            }
            else
                continue;



            ++iCurrentIdx;

            const unsigned iPercent  = 100 * ((float)iCurrentIdx / iTotalCount);

            if (iPercent != iPrevPercent)
            {
                const unsigned iProgress = iMAX_PROGRESS * (iPercent / 100.0);

                if (iProgress != iPrevProgress)
                {
                    for (size_t i = iPrevProgress; i < iProgress; ++i)
                    {
                        sProgress[1 + i] = '=';
                    }

                    iPrevProgress = iProgress;
                }

                if (iPercent == 100)
                {
                    sProgress[iPERCENTAGE_OFFSET-1] = '1';
                    sProgress[iPERCENTAGE_OFFSET  ] = '0';
                    sProgress[iPERCENTAGE_OFFSET+1] = '0';
                }
                else
                {
                    sProgress[iPERCENTAGE_OFFSET  ] = '0' + (iPercent / 10);
                    sProgress[iPERCENTAGE_OFFSET+1] = '0' + (iPercent % 10);
                }

                sProgress[iPERCENTAGE_OFFSET+2] = ' ';
                sProgress[iPERCENTAGE_OFFSET+3] = '%';

                std::printf("\r%s", sProgress.c_str());
            }
        }

        std::printf(
            "\n%llu unmatched, %llu succeeded, %llu failed.\n",
            iUnmatchedCount,
            iSucceededCount,
            iFailedCount
        );
    }



    return 0;
}

