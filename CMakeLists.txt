cmake_minimum_required(VERSION 3.16)

project(qbs)

set (TARGET qbs)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CPPFILES 
    src/filename_input.cpp
    src/input_hierch.cpp
    src/utils.cpp
    src/QBS_batch.cpp
    src/QBS_breport.cpp
    src/QBS_call.cpp
    src/QBS_card_table.cpp
    src/QBS_charth.cpp
    src/QBS_data.cpp
    src/QBS_dormant.cpp
    src/QBS_file.cpp
    src/QBS_import.cpp
    src/QBS_main.cpp
    src/QBS_notes.cpp
    src/QBS_reporter.cpp
    src/QBS_top20.cpp
    src/QBS_window.cpp
)

#FLTK
set(FLTK_DIR ${CMAKE_INSTALL_PREFIX}"/bin"
    CACHE FILEPATH "FLTK installation or build directory")
find_package(FLTK 1.4.4 CONFIG REQUIRED)

# Create QBS_logo.h - note output created in build
add_custom_command(
    OUTPUT "QBS_logo.h"
    COMMAND xxd -i -n qbs_png ${CMAKE_CURRENT_SOURCE_DIR}/qbs.png > QBS_logo.h
)

add_executable(${TARGET} WIN32 MACOSX_BUNDLE)
target_include_directories(${TARGET} PRIVATE ./include)
# Pick up files generated in build directory
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
# Add QBS_logo.h to ensure it gets generated
target_sources(${TARGET} PRIVATE ${CPPFILES} QBS_logo.h)

target_link_libraries(${TARGET} PRIVATE fltk::fltk fltk::images)

install(TARGETS ${TARGET})

