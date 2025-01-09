project(tests C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3 -Wall -Wextra -Wconversion -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -fsanitize=undefined,address -fsanitize-trap")

add_executable(${PROJECT_NAME} src/main.c
        src/game.c
        src/game.h)

target_link_libraries(${PROJECT_NAME} PRIVATE nayla::engine PAL::platform)