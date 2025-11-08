# dbsutl
# ------

# dbsutl_helpers
add_library(dbsutl_helpers)
target_sources(dbsutl_helpers
  PRIVATE
    dbsutl_helpers.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES 
      dbsutl_helpers.h
)

add_executable(dbsutl_helpers.t)
target_sources(dbsutl_helpers.t PRIVATE dbsutl_helpers.t.cpp)
target_link_libraries(dbsutl_helpers.t PRIVATE dbsutl_helpers)
add_test(NAME DbsutlHelpersTest COMMAND dbsutl_helpers.t)
