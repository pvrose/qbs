cmake_minimum_required(VERSION 3.16)

project(QBS VERSION 2.1)

# Add shared CMake modules
include(FetchContent)
FetchContent_Declare(
    zzacmake
    GIT_REPOSITORY https://github.com/pvrose/zzacmake.git
    GIT_TAG main
)
FetchContent_MakeAvailable(zzacmake)
list(APPEND CMAKE_MODULE_PATH "${zzacmake_SOURCE_DIR}/cmake")

include(Gm3zzaProjectDefaults)
include(Gm3zzaVersioning)
include(Gm3zzaZzacommon)
include(Gm3zzaResources)
include(Gm3zzaWindowsRuntime)

## Set target executable and program metadata
set(TARGET qbs)
set(APP_VENDOR "GM3ZZA")
set(APP_NAME "QBS")
set(APP_RC_VALUE "")
string(TIMESTAMP APP_TIMESTAMP UTC)
set(APP_SOURCE_DIR ${QBS_SOURCE_DIR})

## Initialise defaults and settings
gm3zza_project_defaults()
gm3zza_set_default_install_dirs("${APP_NAME}")
gm3zza_split_version(${PROJECT_NAME})

## Set source files (excluding app.cpp which is generated)
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
)

## Set version string for app.cpp.in
set(APP_VERSION "${QBS_VERSION}${APP_RC_VALUE}")


set(DATAFILES
  ${QBS_SOURCE_DIR}/rose.png
)

## Generate Windows icon and resource file
if(MSVC)
  if(NOT DEFINED QBS_ICON_WHITE_THRESHOLD)
    set(QBS_ICON_WHITE_THRESHOLD 240)
  endif()
  
  gm3zza_enable_windows_icon(
    TARGET ${TARGET}
    PNG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rose.png"
    RC_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/qbs.rc.in"
    WHITE_THRESHOLD ${QBS_ICON_WHITE_THRESHOLD}
    OUTPUT_VAR APP_RC_FILE
  )
endif()

## Source file generation in binary tree
gm3zza_generate_app_source(
  INPUT "${QBS_SOURCE_DIR}/src/app.cpp.in"
  OUTPUT_VAR GENERATED_APP_CPP
)

set(ALL_CPPFILES ${GENERATED_APP_CPP} ${CPPFILES})

## Use shared zzacommon
gm3zza_use_zzacommon(COMPONENTS zzaf zzam zzad zzafb GIT_TAG master)

## Create executable
if(MSVC)
  add_executable(${TARGET} WIN32 MACOSX_BUNDLE ${APP_RC_FILE})
else()
  add_executable(${TARGET} WIN32 MACOSX_BUNDLE)
endif()

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

if(MSVC)
  message(STATUS "Including other libraries for MSVC from ${USER_HOME_DIR}")
  target_include_directories(${TARGET} PRIVATE 
    "${USER_HOME_DIR}\\source\\repos\\json\\include"
  )
endif()

target_sources(${TARGET} PRIVATE ${ALL_CPPFILES})

target_link_libraries(${TARGET} PRIVATE 
  ${FLTK_LIBRARIES}
  zzacommon::zzaf
  zzacommon::zzam
  zzacommon::zzad
  zzacommon::zzafb
)

## Stage runtime assets for development
gm3zza_stage_runtime_files(TARGET ${TARGET} FILES ${DATAFILES})

## Copy runtime DLLs for development
gm3zza_copy_runtime_dlls(TARGET ${TARGET})

## Install
install(TARGETS ${TARGET} RUNTIME COMPONENT applications)
gm3zza_install_runtime_files(FILES ${DATAFILES} COMPONENT data)

if(MSVC)
  zzacommon_install_runtime_dlls(DESTINATION bin COMPONENT application)
endif()

