cmake_minimum_required(VERSION 3.20)
project(dbs
  VERSION 0.0.1
  DESCRIPTION "An electronic budgeting system that emulates the envelope budgeting method."
  HOMEPAGE_URL "https://github.com/tjdwill/dbs"
  LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# Qt imports
find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()
# Bloomberg BDE imports
find_package(Threads REQUIRED)
#find_package(bal REQUIRED)
find_package(bdl REQUIRED)
#find_package(bsl REQUIRED)

# Create a symbolic link to point to the correct compile_commands.json directory
if (CMAKE_HOST_UNIX)
  execute_process(
    COMMAND       rm ${CMAKE_SOURCE_DIR}/build
    ERROR_FILE    /dev/null
  )
  execute_process(COMMAND ln -s ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/build)
endif ()
#============Subdirectories====================
add_subdirectory(src)
