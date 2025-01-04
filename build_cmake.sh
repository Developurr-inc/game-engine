./compile_shaders

cmake -DCMAKE_BUILD_TYPE=Debug -S . -B ./cmake-build-debug
cmake --build ./cmake-build-debug
