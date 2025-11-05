cmake_minimum_required(VERSION 3.16)

project(qbs)

set (TARGET qbs)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(VENDOR "GM3ZZA")
set(PROGRAMID "QBS")

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

# set document directory
if(WIN32)
  set(CMAKE_INSTALL_DOCDIR C:/ProgramData/${VENDOR}/${PROGRAMID})
elseif(LINUX)
  set(CMAKE_INSTALL_DOCDIR /etc/${VENDOR}/${PROGRAMID})
endif()

# ser compiler options
if (MSVC)
else()
  add_compile_options(-g)
endif()

set(DATAFILES
  ${CMAKE_CURRENT_SOURCE_DIR}/qbs.png
)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/qbs.png
  COMMAND cp ${DATAFILES} ${CMAKE_CURRENT_BINARY_DIR}/
)

#FLTK
find_package(FLTK CONFIG REQUIRED)

add_executable(${TARGET} WIN32 MACOSX_BUNDLE)
target_include_directories(${TARGET} PRIVATE ./include)
# Add QBS_logo.h to ensure it gets generated
target_sources(${TARGET} PRIVATE ${CPPFILES} ${CMAKE_CURRENT_BINARY_DIR}/qbs.png)

target_link_libraries(${TARGET} PRIVATE fltk::fltk fltk::images)

install(TARGETS ${TARGET})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qbs.png
    DESTINATION ${CMAKE_INSTALL_DOCDIR})

