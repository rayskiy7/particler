set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER   /opt/homebrew/Cellar/mingw-w64/13.0.0_2/toolchain-x86_64/bin/x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER /opt/homebrew/Cellar/mingw-w64/13.0.0_2/toolchain-x86_64/bin/x86_64-w64-mingw32-g++)

set(CMAKE_RC_COMPILER  /opt/homebrew/Cellar/mingw-w64/13.0.0_2/toolchain-x86_64/bin/x86_64-w64-mingw32-windres)

set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++ -pthread")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)
