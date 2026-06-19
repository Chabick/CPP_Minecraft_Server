required dll files are installed with mingw-w64 and are stored under C:\msys64\ucrt64\bin
dlls in question:
- libstdc++-6
- gcc_c_she ...

needs to be build with ninja and MinGW gcc/cc toolchain
don't use clang or msvc or any other toolchain which has a sanitizer or other useful stuff, bc this
program hates me and won't compile otherwise.