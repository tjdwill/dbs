set(debugOptions -Wall -Werror -Wextra -Wpedantic)

# dbsc_uuidstring
add_library(dbsc_uuidstring dbsc_uuidstring.cpp)
target_sources(dbsc_uuidstring 
  PRIVATE
    dbsc_uuidstring.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbsc_uuidstring.h
      dbsc_registerexception.h
)
target_compile_options(dbsc_uuidstring PRIVATE $<$<CONFIG:Debug>:${debugOptions}>)
target_link_libraries(dbsc_uuidstring PUBLIC stduuid)

add_executable(dbsc_uuidstring.t)
target_sources(dbsc_uuidstring.t PRIVATE dbsc_uuidstring.t.cpp)
target_link_libraries(dbsc_uuidstring.t PRIVATE dbsc_uuidstring)
add_test(NAME DbscUuidStringTest COMMAND dbsc_uuidstring.t)

# dbsc_transaction
add_library(dbsc_transaction dbsc_transaction.cpp)
target_sources(dbsc_transaction 
  PRIVATE
    dbsc_transaction.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbsc_transaction.h
)
target_link_libraries(dbsc_transaction 
  PUBLIC
    dbsc_uuidstring
    bdl
    bsl
)

add_executable(dbsc_transaction.t)
target_sources(dbsc_transaction.t PRIVATE dbsc_transaction.t.cpp)
target_link_libraries(dbsc_transaction.t PRIVATE dbsc_transaction bdl)
add_test(NAME DbscTransactionTest COMMAND dbsc_transaction.t)

# dbsc_account
add_library(dbsc_account)
target_sources(dbsc_account
  PRIVATE
    dbsc_account.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbsc_account.h
)
target_link_libraries(dbsc_account 
  PUBLIC 
    dbsc_transaction
    dbsc_uuidstring
    bdl
)
add_executable(dbsc_account.t)
target_sources(dbsc_account.t PRIVATE dbsc_account.t.cpp)
target_link_libraries(dbsc_account.t PRIVATE dbsc_account)
add_test(NAME DbscAccountTest COMMAND dbsc_account.t)

# dbsc_accountbook
add_library(dbsc_accountbook)
target_sources(dbsc_accountbook 
  PRIVATE
    dbsc_accountbook.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbsc_accountbook.h
)
target_include_directories(dbsc_accountbook PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(dbsc_accountbook 
  PUBLIC
    dbsc_account
    dbsc_uuidstring
)

add_executable(dbsc_accountbook.t)
target_sources(dbsc_accountbook.t PRIVATE dbsc_accountbook.t.cpp)
target_link_libraries(dbsc_accountbook.t PRIVATE
  dbsc_accountbook
  dbsc_account
  dbsc_uuidstring
)
add_test(NAME DbscAccountBookTest COMMAND dbsc_accountbook.t)

# dbsc_dbsserializer
## Ensure the header compiles in isolation.
add_library(dbsc_dbsserializer)
target_sources(dbsc_dbsserializer
  PRIVATE
    dbsc_dbsserializer.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbsc_dbsserializer.h
)
target_link_libraries(dbsc_dbsserializer PUBLIC dbsc_accountbook)

# dbsc_dbstomlserializer
add_library(dbsc_tomlserializer) 
target_sources(dbsc_tomlserializer 
  PRIVATE 
    dbsc_tomlserializer.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbsc_tomlserializer.h
)
target_link_libraries(dbsc_tomlserializer PUBLIC
  dbsc_accountbook
  dbsc_account
  dbsc_dbsserializer
  dbsc_transaction
  dbsc_uuidstring
  bdl
  bsl
  tomlplusplus::tomlplusplus
)

add_executable(dbsc_tomlserializer.t)
target_sources(dbsc_tomlserializer.t PRIVATE dbsc_tomlserializer.t.cpp)
target_link_libraries(dbsc_tomlserializer.t 
  PRIVATE 
    dbsc_tomlserializer
    dbsc_accountbook
    dbsc_transaction
    dbsc_account
)
add_test(NAME DbscTomlSerializerTest COMMAND dbsc_tomlserializer.t)
