cmake_minimum_required(VERSION 3.30)
project(game_engine C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -g")

set(VULKAN_HEADERS_ENABLE_MODULE OFF)
set(VULKAN_HEADERS_ENABLE_INSTALL OFF)

add_executable(${CMAKE_PROJECT_NAME} src/main.c src/main.h
        src/vulkan.c
        src/vulkan.h
        src/debug.c
        src/debug.h
        src/glfw.c
        src/glfw.h)

find_package(Vulkan 1.4 REQUIRED)
find_package(glfw3 3.4 REQUIRED)

target_link_libraries(${CMAKE_PROJECT_NAME} Vulkan::Vulkan glfw)