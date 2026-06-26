cmake_minimum_required(VERSION 3.30)
project(markov-music)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)
file(GLOB MY_SOURCES "src/*.cpp")
list(REMOVE_ITEM MY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
list(REMOVE_ITEM MY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/midi_processor.cpp")

add_executable(markov-music src/main.cpp ${MY_SOURCES})

add_executable(score-tests ../tests/score_tests.cpp ${MY_SOURCES})
