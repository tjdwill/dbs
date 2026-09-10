cmake_minimum_required( VERSION 3.23 )
project( dbs
  VERSION 0.2.0
  DESCRIPTION "An electronic budgeting system that emulates the envelope budgeting method."
  HOMEPAGE_URL "https://github.com/tjdwill/dbs"
  LANGUAGES CXX
)

# Register CMakeModules directory to access this project's modules.
list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules )

enable_testing()

# Qt imports
find_package( Qt6 REQUIRED COMPONENTS Core Gui Widgets )
qt_standard_project_setup()
set( CMAKE_AUTORCC ON )

find_package( tomlplusplus REQUIRED )
find_package( stduuid REQUIRED )

# Bloomberg BDE imports
find_package( Threads REQUIRED )
find_package( bdl REQUIRED )
find_package( bsl REQUIRED )

#============Subdirectories====================
option( DBS_BUILD_TUTORIALS "Build the components in the tutorialSpace directory" OFF )
if( DBS_BUILD_TUTORIALS )
  add_subdirectory( tutorialSpace )
endif()

add_library( dbsCompileOptionsInternal INTERFACE )
if( MSVC )
  target_compile_options( dbsCompileOptionsInternal INTERFACE /W4 )
else()
  target_compile_options( dbsCompileOptionsInternal INTERFACE -Wall -Wextra -Wpedantic )
endif()

add_subdirectory( src )

if( CMAKE_HOST_UNIX )
  # Create a symbolic link to point to the correct compile_commands.json directory
  execute_process(
    COMMAND       rm ${PROJECT_SOURCE_DIR}/build
    ERROR_FILE    /dev/null
  )
  execute_process( COMMAND ln -s ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/build )
elseif( CMAKE_HOST_WIN32 )
  # Copy the compile_commands.json file anytime CMake reconfigures. One way to
  # detect this is to detect if CMake-related files change.
  file( GLOB command_dependencies 
    "CMakeLists.txt"
    "src/**/CMakeLists.txt" 
    "tutorialSpace/CMakeLists.txt" 
    "CMakeModules/*.cmake"
    "src/**/*.cmake"
  )
  add_custom_command( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    VERBATIM
    COMMENT "Copying compilation database."
    COMMAND powershell "Copy-Item -Path ${CMAKE_BINARY_DIR}/compile_commands.json -Destination ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json"
    DEPENDS
      ${command_dependencies}
  )

  # Create a custom target that depends on the generated file. This target is
  # then added as a dependency of this project's main target, forcing the file
  # generation before the main target is built.
  add_custom_target( CopyCompilationDatabase DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json )
  add_dependencies( dbs CopyCompilationDatabase )
else ()
endif()

install( FILES LICENSE.txt
  TYPE DATA
)
