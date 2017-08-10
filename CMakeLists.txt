cmake_minimum_required(VERSION 2.8)
project(sparkle)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_compile_options(-g)

set(SOURCE_FILES src/main.cpp src/bounded_buffer.h src/actor.h src/producer.h src/reactor.h src/stateful_producer.h src/stateful_reactor.h src/actor_builder.h src/group.h src/actor_system.h)
add_executable(sparkle ${SOURCE_FILES})

target_link_libraries(sparkle pthread)

# see https://cmake.org/cmake/help/latest/module/FindBoost.html
find_package(Boost COMPONENTS REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

target_link_libraries(sparkle ${Boost_LIBRARIES})
