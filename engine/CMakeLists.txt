project(engine C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ObjC -fPIC -g3 -Wall -Wextra -Wconversion -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -fsanitize=undefined,address -fsanitize-trap")

add_library(${PROJECT_NAME} SHARED
        src/defines.h
        src/core/logger.c
        include/core/logger.h
        src/core/asserts.c
        include/core/asserts.h
        src/platform/platform.h
        src/platform/windows/platform_win32.c
        src/platform/windows/platform_win32.h
        src/platform/linux/platform_linux.c
        src/platform/linux/platform_linux.h
        src/platform/macOS/platform_macos.m)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE objc "-framework AppKit -framework QuartzCore")

add_library(nayla::engine ALIAS ${PROJECT_NAME})