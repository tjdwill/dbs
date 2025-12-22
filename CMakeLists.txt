cmake_minimum_required(VERSION 3.23)
project(dbs
  VERSION 0.1.0
  DESCRIPTION "An electronic budgeting system that emulates the envelope budgeting method."
  HOMEPAGE_URL "https://github.com/tjdwill/dbs"
  LANGUAGES CXX)

enable_testing()
#===================Top-Config==========================
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  if(MSVC)
    # warning level 4
    add_compile_options(/W4)
  else()
    add_compile_options(-Wall -Wextra -Wpedantic)
  endif()
endif()

message("Prefix path: $ENV{CMAKE_PREFIX_PATH}")

# Qt imports
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()
set(CMAKE_AUTORCC ON)

find_package(tomlplusplus REQUIRED)
find_package(stduuid REQUIRED)

# Bloomberg BDE imports
find_package(Threads REQUIRED)
#find_package(bal REQUIRED)
find_package(bdl REQUIRED)
find_package(bsl REQUIRED)

#============Subdirectories====================
set(DBS_THIRDPARTY_BASE_DIR ${PROJECT_SOURCE_DIR}/3rdparty)

option(BUILD_TUTORIALS "Build the components in the tutorialSpace directory" OFF )
if (BUILD_TUTORIALS)
  add_subdirectory(tutorialSpace)
endif()
add_subdirectory(src)

if( CMAKE_HOST_UNIX )
  # Create a symbolic link to point to the correct compile_commands.json directory
  execute_process(
    COMMAND       rm ${PROJECT_SOURCE_DIR}/build
    ERROR_FILE    /dev/null
  )
  execute_process(COMMAND ln -s ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/build)
elseif( CMAKE_HOST_WIN32 )
  add_custom_target(copyCompilationDatabase ALL
    COMMAND powershell "Copy-Item -Path ${CMAKE_BINARY_DIR}/compile_commands.json -Destination ${CMAKE_SOURCE_DIR}/compile_commands.json"
    VERBATIM
  )
else ()
endif()

install(FILES LICENSE.txt
  DESTINATION ${CMAKE_INSTALL_PREFIX}
)