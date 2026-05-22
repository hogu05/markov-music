cmake_minimum_required(VERSION 3.30)
project(markov-music)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(libs/midifile/include)
file(GLOB LIB_SOURCES "libs/midifile/src/*.cpp")

include_directories(include)
file(GLOB MY_SOURCES "src/*.cpp")
list(REMOVE_ITEM MY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_executable(markov-music src/main.cpp ${MY_SOURCES} ${LIB_SOURCES})

add_executable(score-tests ../tests/score_tests.cpp ${MY_SOURCES} ${LIB_SOURCES})
