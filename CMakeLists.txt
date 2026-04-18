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
set(APP_SOURCE_DIR ${QBS_SOURCE_DIR})

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

include(FetchContent)
FetchContent_Declare(
  zzacommon
  GIT_REPOSITORY https://github.com/pvrose/zzacommon.git
  GIT_TAG master
  FIND_PACKAGE_ARGS NAMES zzacommon COMPONENTS zzaf zzam zzad zzafb
)
FetchContent_MakeAvailable(zzacommon)

configure_file(
  ${QBS_SOURCE_DIR}/src/app.cpp.in
  ${QBS_SOURCE_DIR}/src/app.cpp
  @ONLY
)

set(SMAKE_BUILD_RPATH )
add_executable(${TARGET} WIN32 MACOSX_BUNDLE)
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
if(MSVC)
  file(COPY ${DATAFILES}
     DESTINATION ${QBS_BINARY_DIR}/${CMAKE_BUILD_TYPE}
  )
else()
  file(COPY ${DATAFILES}
     DESTINATION ${QBS_BINARY_DIR}
  )
endif()

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

