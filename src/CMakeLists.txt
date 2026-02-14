cmake_minimum_required(VERSION 3.30)
project(markov_music)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(libs/midifile/include)
file(GLOB LIB_SOURCES "libs/midifile/src/*.cpp")

include_directories(include)
file(GLOB MY_SOURCES "src/*.cpp")

add_executable(markov_music ${MY_SOURCES} ${LIB_SOURCES})
