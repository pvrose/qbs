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
if(MSVC)
  set(CMAKE_INSTALL_RPATH C:/ProgramData/${VENDOR}/${PROGRAMID})
else()
  set(CMAKE_INSTALL_RPATH /etc/${VENDOR}/${PROGRAMID})
endif()

# ser compiler options
if (MSVC)
  add_compile_options(/MD)
else()
  add_compile_options(-g)
endif()

set(DATAFILES
  ${CMAKE_CURRENT_SOURCE_DIR}/qbs.png
)

#FLTK - work-round for MSVC 
if (0)
  set(FLTK_INCLUDE_DIR "C:\\Users\\pvros\\source\\repos\\fltk\\out\\install\\x64-Release\\include")
  set(FLTK_LIBRARIES "C:\\Users\\pvros\\source\\repos\\fltk\\out\\install\\x64-Release\\lib\\fltk.lib;C:\\Users\\pvros\\source\\repos\\fltk\\out\\install\\x64-Release\\lib\\fltk_images.lib;C:\\Users\\pvros\\source\\repos\\fltk\\out\\install\\x64-Release\\lib\\fltk_jpeg.lib;C:\\Users\\pvros\\source\\repos\\fltk\\out\\install\\x64-Release\\lib\\fltk_png.lib;C:\\Users\\pvros\\source\\repos\\fltk\\out\\install\\x64-Release\\lib\\fltk_z.lib;gdiplus.lib;Ws2_32.lib;User32.lib;gdi32.lib;comctl32.lib")
else()
 set(FLTK_DIR "C:\\Users\\pvros\\source\\repos\\fltk\\build")
   set(CMAKE_MODULE_PATH ../ ./)
    find_package(FLTK CONFIG REQUIRED)
    if (FLTK_FOUND)
      message(STATUS "FLTK found: ${FLTK_VERSION}")
      message(STATUS "FLTK include dir: ${FLTK_INCLUDE_DIR}")
#      set(FLTK_LIBRARIES fltk::fltk fltk::fltk_images)
    else()
      message(FATAL_ERROR "FLTK not found")
    endif()
endif()

set(SMAKE_BUILD_RPATH )
add_executable(${TARGET} WIN32 MACOSX_BUNDLE)
target_include_directories(${TARGET} PRIVATE 
  ./include
  ${FLTK_INCLUDE_DIR}
)
# Add QBS_logo.h to ensure it gets generated
target_sources(${TARGET} PRIVATE ${CPPFILES})
if(MSVC)
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/qbs.png
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
else()
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/qbs.png
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

target_link_libraries(${TARGET} PRIVATE ${FLTK_LIBRARIES})

install(TARGETS ${TARGET})
install(FILES ${DATAFILES}
    DESTINATION ${CMAKE_INSTALL_RPATH})

