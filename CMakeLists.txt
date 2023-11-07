cmake_minimum_required(VERSION 3.27.7)

set(CMAKE_C_COMPILER gcc-12)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic")
set(CMAKE_FIND_FRAMEWORK ONLY)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

project(Browser)

file(GLOB_RECURSE ASSETS "assets/**/*.*")
file(COPY assets DESTINATION ${EXECUTABLE_OUTPUT_PATH})

file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "headers/*.h")

add_executable(executable ${SOURCES} ${HEADERS} ${ASSETS})
target_include_directories(executable PRIVATE "headers")

find_package(SDL2 REQUIRED)
target_include_directories(executable PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(executable ${SDL2_LIBRARIES})

find_package(SDL2_ttf REQUIRED)
target_include_directories(executable PRIVATE /Library/Frameworks/SDL2_ttf.framework/Headers)
target_link_libraries(executable SDL2_ttf::SDL2_ttf)
