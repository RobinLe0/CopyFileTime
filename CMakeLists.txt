cmake_minimum_required (VERSION 3.8)

# hot reload for MSVC (if supported)
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# ensure CMAKE_BUILD_TYPE is set if it's not set by the preset
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")  # Default to Release if not set
endif()

project ("CopyFileTime")


# set the application output directory (Windows: .exe)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/${ROBINLE_BINPATH}")
# set the static library output directory (Windows: .lib, Linux: .a)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/${ROBINLE_BINPATH}")
# set the dynamic library output directory (Windows: .dll, Linux: .so)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/${ROBINLE_BINPATH}")

add_executable (CopyFileTime "src/main.cpp" "src/Options.cpp" "src/CopyTimes.cpp" "res/version.rc")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CopyFileTime PROPERTY CXX_STANDARD 20)
endif()
