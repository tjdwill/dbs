# dbsutl
# ------
include(GenerateExportHeader)

# Define a target that allows the propagation of the CMake-generated export header.
add_library(dbsutl)
generate_export_header(dbsutl
  EXPORT_FILE_NAME dbsutl_sharedapi.h
  EXPORT_MACRO_NAME DBSUTL_API
  NO_EXPORT_MACRO_NAME DBSUTL_INTERNAL
)
target_sources(dbsutl 
  PRIVATE
    dbsutl_helpers.cpp
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
    FILES
      dbsutl_helpers.h
      ${CMAKE_CURRENT_BINARY_DIR}/dbsutl_sharedapi.h
)

add_executable(dbsutl_helpers.t)
target_sources(dbsutl_helpers.t PRIVATE dbsutl_helpers.t.cpp)
target_link_libraries(dbsutl_helpers.t PRIVATE dbsutl)
add_test(NAME DbsutlHelpersTest COMMAND dbsutl_helpers.t)
