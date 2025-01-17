# Define the project and specify the C standard
project(tests C)

# Set the C standard to C23
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ------------------------------
# Compiler Flags and Options
# ------------------------------
# Global compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wconversion -Wdouble-promotion -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion")

# ------------------------------
# Debug Configuration
# ------------------------------
# Define debug flags if building in debug mode
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -fPIC -DDEBUG")
endif ()

# ------------------------------
#  Configuration
# ------------------------------
# Define source files
set(SOURCES
        src/main.c
        src/game.c
        src/game.h
)

# ------------------------------
# Create Executable
# ------------------------------
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/NAYLA/include
        ${CMAKE_SOURCE_DIR}/PAL/include
)

# ------------------------------
# Linking Libraries
# ------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE NAYLA::engine)