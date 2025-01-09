project(engine C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -g3 -Wall -Wextra -Wconversion -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -fsanitize=undefined,address -fsanitize-trap")

set(SOURCES
        include/game_types.h
        include/entry.h
        include/core/logger.h
        include/core/application.h
        src/defines.h
        src/core/logger.c
        src/core/asserts.c
        src/core/asserts.h
        src/core/application.c
        src/core/n_memory.h
        src/core/n_memory.c
        src/platform/platform.h
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE PAL::platform)

add_library(nayla::engine ALIAS ${PROJECT_NAME})