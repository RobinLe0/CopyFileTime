#include <cstdio>

#include "Options.hpp"

int wmain(int argc, wchar_t* argv[])
{
    Options opt(argc, argv);

    std::printf("Hello CMake!\n");

    return 0;
}

