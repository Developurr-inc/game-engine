project(tests C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -g")

add_executable(${PROJECT_NAME} src/main.c)

target_link_libraries(${PROJECT_NAME})