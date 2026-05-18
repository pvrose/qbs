cmake_minimum_required(VERSION 3.16)

project(QBS VERSION 2.1)

## Set target executable
set (TARGET qbs)
if (NOT MSVC)
  set(BUILD_SHARED_LIBS OFF)
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT MSVC)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

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
set(APP_SOURCE_DIR ${QBS_SOURCE_DIR})

## Ensure all version components are defined for the resource file
if(NOT DEFINED QBS_VERSION_MAJOR)
  set(QBS_VERSION_MAJOR 0)
endif()
if(NOT DEFINED QBS_VERSION_MINOR)
  set(QBS_VERSION_MINOR 0)
endif()
if(NOT DEFINED QBS_VERSION_PATCH)
  set(QBS_VERSION_PATCH 0)
endif()
if(NOT DEFINED QBS_VERSION_TWEAK)
  set(QBS_VERSION_TWEAK 0)
endif()

## Set source files
set(CPPFILES 
    ${QBS_SOURCE_DIR}/src/app.cpp
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
)

# set document directory
# set directory to install executable, reference data and documents.
if (MSVC)
  set(CMAKE_INSTALL_RPATH etc)
else()
  set(CMAKE_INSTALL_RPATH etc/${TARGET})
endif()
message(STATUS "Reference data will be copied to ${CMAKE_INSTALL_RPATH}")

# ser compiler options
if (MSVC)
#  add_compile_options(/MD)
else()
  add_compile_options(-g)
endif()

set(DATAFILES
  ${QBS_SOURCE_DIR}/rose.png
)

# Generate Windows icon and resource file
if (MSVC)
  # Include the icon helper functions
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/icon_helper.cmake)

  # Optional: Set white threshold for transparency (default: 250, range: 0-255)
  # Higher values = more aggressive white removal (e.g., 240 removes near-white)
  # Lower values = only pure white removed (e.g., 254 removes almost pure white)
  if(NOT DEFINED QBS_ICON_WHITE_THRESHOLD)
    set(QBS_ICON_WHITE_THRESHOLD 240)
  endif()

  # Generate icon and resource file using the helper function
  generate_windows_icon_and_resource(
    PNG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rose.png"
    RC_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/qbs.rc.in"
    WHITE_THRESHOLD ${QBS_ICON_WHITE_THRESHOLD}
  )

  # The function exports ${PROJECT_NAME}_RC_FILE which we'll use below
  # For project QBS, this will be ${QBS_RC_FILE}
  message(STATUS "Using resource file: ${QBS_RC_FILE}")
endif()

# Add ZZACOMMON
# Try to find zzacommon locally first (sibling directory), fall back to downloading
if(CMAKE_BUILD_TYPE)
  # Single-config generator (Unix Makefiles, Ninja): use specific build type
  string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_LOWER)
  list(APPEND CMAKE_PREFIX_PATH 
    "${CMAKE_CURRENT_SOURCE_DIR}/../zzacommon/out/build/x64-${BUILD_TYPE_LOWER}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../zzacommon/out/install/x64-${BUILD_TYPE_LOWER}"
  )
endif()
# Multi-config generator (Visual Studio) or fallback: include both
list(APPEND CMAKE_PREFIX_PATH 
  "${CMAKE_CURRENT_SOURCE_DIR}/../zzacommon/out/build/x64-debug"
  "${CMAKE_CURRENT_SOURCE_DIR}/../zzacommon/out/build/x64-release"
  "${CMAKE_CURRENT_SOURCE_DIR}/../zzacommon/out/install/x64-debug"
  "${CMAKE_CURRENT_SOURCE_DIR}/../zzacommon/out/install/x64-release"
  "${CMAKE_CURRENT_SOURCE_DIR}/../zzacommon/build"
  "${CMAKE_CURRENT_SOURCE_DIR}/../zzacommon/install"
)
include(FetchContent)
set(ZZACOMMON_REQUIRED_COMPONENTS zzaf zzam zzad zzafb)
set(ZZACOMMON_BUILD_COMPONENTS "${ZZACOMMON_REQUIRED_COMPONENTS}" CACHE STRING "Components to build when zzacommon is used via FetchContent" FORCE)
FetchContent_Declare(
  zzacommon
  GIT_REPOSITORY https://github.com/pvrose/zzacommon.git
  GIT_TAG master
  FIND_PACKAGE_ARGS NAMES zzacommon COMPONENTS ${ZZACOMMON_REQUIRED_COMPONENTS}
)
FetchContent_MakeAvailable(zzacommon)

configure_file(
  ${QBS_SOURCE_DIR}/src/app.cpp.in
  ${QBS_SOURCE_DIR}/src/app.cpp
  @ONLY
)

set(SMAKE_BUILD_RPATH )
if (MSVC)
  add_executable(${TARGET} WIN32 MACOSX_BUNDLE ${QBS_RC_FILE})
else()
  add_executable(${TARGET} WIN32 MACOSX_BUNDLE)
endif()
# Ensure external projects are built before linking qbs

add_dependencies(${TARGET} 
  zzacommon::zzaf 
  zzacommon::zzam 
  zzacommon::zzad
  zzacommon::zzafb
)

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
file(COPY ${DATAFILES}
   DESTINATION ${QBS_BINARY_DIR}
)

target_link_libraries(${TARGET} PRIVATE 
  ${FLTK_LIBRARIES}
  zzacommon::zzaf
  zzacommon::zzam
  zzacommon::zzad
  zzacommon::zzafb
)

# Copy runtime DLLs to build directory for MSVC
if (MSVC)
  # Get the parent project's binary directory
  set(RUNTIME_OUTPUT_DIR ${CMAKE_BINARY_DIR})

  # Use CMake's automatic runtime DLL detection
  add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_RUNTIME_DLLS:${TARGET}>
      ${RUNTIME_OUTPUT_DIR}
    COMMAND_EXPAND_LISTS
    COMMENT "Copying runtime DLLs to ${RUNTIME_OUTPUT_DIR}"
  )

endif()

install(TARGETS ${TARGET})
install(FILES ${DATAFILES}
    DESTINATION ${CMAKE_INSTALL_RPATH})

# install DLLs
if (MSVC)
    zzacommon_install_runtime_dlls(DESTINATION bin COMPONENT application)
endif()
