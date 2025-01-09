project(PAL C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ObjC -fPIC -g3 -Wall -Wextra -Wconversion -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -fsanitize=undefined,address -fsanitize-trap")

set(SOURCES
        include/platform.h
        include/PAL/console.h
        include/PAL/memory.h
        include/PAL/time.h
        include/PAL/types.h
        include/PAL/window.h
        src/linux/console_linux.c
        src/linux/memory_linux.c
        src/linux/time_linux.c
        src/macOS/console_macOS.c
        src/macOS/memory_macOS.c
        src/macOS/time_macOS.c
        src/windows/console_win32.c
        src/windows/memory_win32.c
        src/windows/time_win32.c
        src/linux/window_linux.c
        src/macOS/window_macOS.m
        src/windows/window_win32.c
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} PRIVATE objc "-framework AppKit -framework QuartzCore")

add_library(PAL::platform ALIAS ${PROJECT_NAME})