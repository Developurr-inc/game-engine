# Define the project and specify the C standard
project(NAYLA C)

# Set the C standard to C23
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add PAL_BUILDING_DLL for library compilation
set(COMPILE_DEFINITIONS "${COMPILE_DEFINITIONS} -DPAL_BUILDING_DLL")

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
        include/NAYLA/application.h
        include/NAYLA/export.h
        include/NAYLA/logger.h
        include/NAYLA/memory.h
        include/NAYLA/types.h
        include/entry.h
        src/core/_asserts.h
        src/core/_event.h
        src/core/_logger.h
        src/core/_memory.h
        src/core/application.c
        src/core/asserts.c
        src/core/event.c
        src/core/logger.c
        src/core/memory.c
        src/debug.h
        src/defines.h
)

# ------------------------------
# Create Shared Library
# ------------------------------
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC include)

# ------------------------------
# Linking Libraries
# ------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE PAL::platform)

# ------------------------------
# Export Configuration (Header)
# ------------------------------
# Configure export header with the appropriate export macro
#configure_file(${CMAKE_SOURCE_DIR}/include/PAL/export.h.in ${EXPORT_HEADER_PATH})

# ------------------------------
# Create Platform Alias
# ------------------------------
add_library(NAYLA::engine ALIAS ${PROJECT_NAME})
