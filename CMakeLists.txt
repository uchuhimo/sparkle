cmake_minimum_required(VERSION 2.8)
project(sparkle)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_compile_options(-g)

file(GLOB SOURCE_FILES
    "src/*.h"
    "src/*.cpp"
)

add_executable(sparkle ${SOURCE_FILES})

target_link_libraries(sparkle
        ${CONAN_LIBS}
        pthread)
