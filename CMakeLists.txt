cmake_minimum_required(VERSION 3.10)

project(chip-8 VERSION 1.0)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(chip-8 ${SRC_FILES})

find_package(SFML REQUIRED network audio graphics window system)

target_include_directories(chip-8 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(chip-8 PUBLIC sfml-network sfml-audio sfml-graphics sfml-window sfml-system 
                        ${GLFW3_LIBRARY} ${GLEW_LIBRARIES})

