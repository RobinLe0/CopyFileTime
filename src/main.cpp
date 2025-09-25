#include <cstdio>
#include <filesystem>
#include <functional>

#include "Options.hpp"
#include "CopyTimes.hpp"



void PrintHelp(FILE *stream);



int wmain(int argc, wchar_t* argv[])
{
    namespace fs = std::filesystem;

    Options opt(argc, argv);

    if (!opt.ok())
    {
        if (opt.help())
            PrintHelp(stderr);
        else
            std::fprintf(stderr, "%s\n", opt.errorMessage().c_str());
        return 1;
    }

    if (opt.help())
    {
        PrintHelp(stdout);
        return 0;
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

        if (opt.verbose())
            std::printf("Counting files... ");
        uint64_t iTotalCount = 0;
        for (const auto &item : fs::recursive_directory_iterator(opt.sourcePath()))
        {
            if (item.is_regular_file() || item.is_directory())
                ++iTotalCount;
        }
        if (opt.verbose())
            std::printf("Found %llu.\n", iTotalCount);
        if (iTotalCount == 0)
        {
            std::printf("The source directory contained no elements.\nExiting.\n");
            return 0;
        }


        std::string sProgress =
            "[                                 00 %                                 ]";
        constexpr unsigned iMAX_PROGRESS      = 70;
        constexpr unsigned iPERCENTAGE_OFFSET = 34;

        unsigned iPrevPercent  = 0;
        unsigned iPrevProgress = 0;

        if (opt.verbose())
            std::printf("Copying times...\n\n");
        if (opt.showPercentage())
            std::printf("%s", sProgress.c_str());
        uint64_t iCurrentIdx = 0;

        constexpr char szLINEBREAK[] = "\n";
        const char *szLinebreakBeforeResult =
            opt.showPercentage()
            ? szLINEBREAK
            : szLINEBREAK + 1;
        const auto fnReportResult =
            opt.verbose()
            ? std::function<void(const char *, const char *)>(
                [&](const char *szFormat, const char *szPath)
                {
                    std::printf(szFormat, szLinebreakBeforeResult, szPath);
                }
            )
            : [](const char *szFormat, const char *szPath)
                {
                    (void)szFormat;
                    (void)szPath;
                };

        const auto fnUpdateProgressBar =
            opt.showPercentage()
            ? std::function<void(void)>(
                [&]()
                {
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
                            sProgress[iPERCENTAGE_OFFSET] = '0';
                            sProgress[iPERCENTAGE_OFFSET+1] = '0';
                        }
                        else
                        {
                            sProgress[iPERCENTAGE_OFFSET] = '0' + (iPercent / 10);
                            sProgress[iPERCENTAGE_OFFSET+1] = '0' + (iPercent % 10);
                        }

                        sProgress[iPERCENTAGE_OFFSET+2] = ' ';
                        sProgress[iPERCENTAGE_OFFSET+3] = '%';

                        std::printf("\r%s", sProgress.c_str());
                    }
                }
            )
            : []() {};

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
                    fnReportResult("%sFile \"%s\": No match.\n", pathDest.string().c_str());
                }
                else
                {
                    if (!CopyFileTimes(item.path().wstring(), pathDest.wstring()))
                        ++iFailedCount;
                    else
                    {
                        ++iSucceededCount;
                        fnReportResult("%sFile \"%s\": Succeeded.\n", pathDest.string().c_str());
                    }
                }
            }
            else if (item.is_directory())
            {
                if (!fs::is_directory(pathDest))
                {
                    ++iUnmatchedCount;
                    fnReportResult("%sDirectory \"%s\": No match.\n", pathDest.string().c_str());
                }
                else
                {
                    if (!CopyDirTimes(item.path().wstring(), pathDest.wstring()))
                        ++iFailedCount;
                    else
                    {
                        ++iSucceededCount;
                        fnReportResult("%sDirectory \"%s\": Succeeded.\n", pathDest.string().c_str());
                    }
                }
            }
            else
                continue;



            fnUpdateProgressBar();
        }

        std::printf(
            "\n\n%llu unmatched, %llu succeeded, %llu failed.\n",
            iUnmatchedCount,
            iSucceededCount,
            iFailedCount
        );
    }



    return 0;
}



void PrintHelp(FILE *stream)
{
    std::fprintf(stream,
        "Usage: %s SOURCE DEST [OPTION]\n"
        "   or: %s {-h|--help}\n"
        "Copy the created, modified and last access time from one file or directory to another.\n"
        "\n"
        "  -h, --help                   display this help and exit\n"
        "  -n, --no-percentage          don't print the current progress percentage\n"
        "  -v, --verbose                show what's being done in more detail\n"
        "\n"
        "Both SOURCE and DEST must exist. They can be either files or directories. However, they\n"
        "have to be the same type: If SOURCE is a directory, DEST must also be a directory, and\n"
        "if SOURCE is a file, DEST must also be a file.\n"
        "\n"
        "If the given paths are directories, the DEST directory itself is not modified; only its\n"
        "child elements are.\n"
        "\n",
        Parameters::szAPPLICATION_NAME,
        Parameters::szAPPLICATION_NAME
    );
}
