cmake_minimum_required(VERSION 3.16)

project(QBS VERSION 2.0.3)

## Set target executable
set (TARGET qbs)
if (NOT MSVC)
  set(BUILD_SHARED_LIBS OFF)
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

## Set home directory per OS.
if(MSVC)
  set(USER_HOME_DIR $ENV{USERPROFILE})
else()
  set(USER_HOME_DIR $ENV{HOME})
endif()

## Set program ident
set(APP_VENDOR "GM3ZZA")
set(APP_NAME "QBS")
string(TIMESTAMP APP_TIMESTAMP UTC)
set(APP_VERSION "${QBS_VERSION}")
configure_file(
  ${QBS_SOURCE_DIR}/src/ident.cpp.in
  ${QBS_SOURCE_DIR}/src/ident.cpp
  @ONLY
)

## Set source files
set(CPPFILES 
    ${QBS_SOURCE_DIR}/src/QBS_batch.cpp
    ${QBS_SOURCE_DIR}/src/QBS_breport.cpp
    ${QBS_SOURCE_DIR}/src/QBS_call.cpp
    ${QBS_SOURCE_DIR}/src/QBS_card_table.cpp
    ${QBS_SOURCE_DIR}/src/QBS_charth.cpp
    ${QBS_SOURCE_DIR}/src/QBS_data.cpp
    ${QBS_SOURCE_DIR}/src/QBS_dormant.cpp
    ${QBS_SOURCE_DIR}/src/QBS_file.cpp
    ${QBS_SOURCE_DIR}/src/QBS_import.cpp
    ${QBS_SOURCE_DIR}/src/QBS_main.cpp
    ${QBS_SOURCE_DIR}/src/QBS_notes.cpp
    ${QBS_SOURCE_DIR}/src/QBS_reporter.cpp
    ${QBS_SOURCE_DIR}/src/QBS_top20.cpp
    ${QBS_SOURCE_DIR}/src/QBS_window.cpp
    ${QBS_SOURCE_DIR}/src/ident.cpp
)

# set document directory
if(MSVC)
  set(CMAKE_INSTALL_RPATH C:/ProgramData/${APP_VENDOR}/${APP_NAME})
else()
  set(CMAKE_INSTALL_RPATH /etc/${APP_VENDOR}/${APP_NAME})
endif()

# ser compiler options
if (MSVC)
#  add_compile_options(/MD)
else()
  add_compile_options(-g)
endif()

set(DATAFILES
  ${QBS_SOURCE_DIR}/qbs.png
)

# FLTK - 
# TODO: Should this be an ExternalProject?
# MSVC work-round as FLTK built in user.
message(STATUS "FLTK_DIR: ${FLTK_DIR}")
if (MSVC AND NOT FLTK_DIR)
  set(FLTK_DIR "${USER_HOME_DIR}\\source\\repos\\fltk\\build")
endif()
# Find FLTK.
set(CMAKE_MODULE_PATH ../ ./)
find_package(FLTK CONFIG REQUIRED)
if (FLTK_FOUND)
  message(STATUS "FLTK found: ${FLTK_VERSION}")
  message(STATUS "FLTK include dir: ${FLTK_INCLUDE_DIR}")
  message(STATUS "FLTK libraries: ${FLTK_LIBRARIES}")
else()
  message(FATAL_ERROR "FLTK not found")
endif()

# libboost
if (MSVC)
  find_package(Boost CONFIG REQUIRED COMPONENTS filesystem system)
else()
  find_package(Boost CONFIG REQUIRED COMPONENTS ALL)
endif()
if (Boost_FOUND)
  message(STATUS "Boost found: ${Boost_VERSION}")
  message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
  message(STATUS "Boost includes: ${Boost_INCLUDE_DIRS}")
endif()

# Add ZZACOMMON
set(ZZACOMMON_DOX OFF)
add_subdirectory(zzacommon)
message(STATUS "ZZACOMMON include dir: ${ZZACOMMON_INCLUDE_DIR}")

set(SMAKE_BUILD_RPATH )
add_executable(${TARGET} WIN32 MACOSX_BUNDLE)
# Ensure external projects are built before linking qbs

add_dependencies(${TARGET} zzacommon)

target_include_directories(${TARGET} PRIVATE 
 ${QBS_SOURCE_DIR}/include
 ${ZZACOMMON_INCLUDE_DIR}
 ${FLTK_INCLUDE_DIR}
)
## Extra include directories required on MSVC
if (MSVC)
  message(STATUS "Including other libraries for MSVC from ${USER_HOME_DIR}")
  target_include_directories(${TARGET} PRIVATE 
    "${USER_HOME_DIR}\\source\\repos\\json\\include"
  )
endif()
target_sources(${TARGET} PRIVATE ${CPPFILES})
if(MSVC)
  file(COPY ${QBS_SOURCE_DIR}/qbs.png
     DESTINATION ${QBS_BINARY_DIR}/${CMAKE_BUILD_TYPE}
  )
else()
  file(COPY ${QBS_SOURCE_DIR}/qbs.png
     DESTINATION ${QBS_BINARY_DIR}
  )
endif()

target_link_libraries(${TARGET} PRIVATE 
  zzacommon
  ${Boost_LIBRARIES}
  ${FLTK_LIBRARIES}
)

install(TARGETS ${TARGET})
install(FILES ${DATAFILES}
    DESTINATION ${CMAKE_INSTALL_RPATH})

