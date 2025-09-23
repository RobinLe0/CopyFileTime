# RobinLe CMake Project Template

This repo acts as a template for my CMake-based projects.


## Directories

This CMake project is configured to...

- create the project files in the default `out` directory
- create the binary files in a `bin` directory: `bin/<config>/<architecture>`
  - `<config>` is either `Debug` or `Release`
  - `<architecture>` is `win-x64` or `win-x86` for Windows and `linux-x64` or `linux-x86` for Linux.


## Helper Scripts

In addition to the CMake-related files, the root directory of this repo
also contains some scripts for both Windows (`.bat`) and Linux (no file
extension):

| Script  | Action |
|---------|--------|
| `build` | Create + compile the project (Release mode, both 64 and 32 bit). |
| `run`   | Run the 64-bit binary. Any call parameters of the script are used as call parameters for the binary. |
