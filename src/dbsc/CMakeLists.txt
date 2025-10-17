set(debugOptions -Wall -Werror -Wextra -Wpedantic)

# dbsc_uuidstring
add_library(dbscUuidString dbsc_uuidstring.cpp)
target_sources(dbscUuidString 
  PRIVATE
    dbsc_uuidstring.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbsc_uuidstring.h
      dbsc_registerexception.h
)
target_compile_options(dbscUuidString PRIVATE $<$<CONFIG:Debug>:${debugOptions}>)
target_link_libraries(dbscUuidString PUBLIC stduuid)

add_executable(testDbscUuidString)
target_sources(testDbscUuidString PRIVATE dbsc_uuidstring.t.cpp)
target_link_libraries(testDbscUuidString PRIVATE dbscUuidString)
add_test(NAME dbscUuidString.t COMMAND testDbscUuidString)

# dbsc_transaction
add_library(dbscTransaction dbsc_transaction.cpp)
target_sources(dbscTransaction 
  PRIVATE
    dbsc_transaction.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbsc_transaction.h
)
target_link_libraries(dbscTransaction 
  PUBLIC
    dbscUuidString
    bdl
    bsl
)

add_executable(testDbscTransaction)
target_sources(testDbscTransaction PRIVATE dbsc_transaction.t.cpp)
target_link_libraries(testDbscTransaction PRIVATE dbscTransaction bdl)
add_test(NAME dbscTransaction.t COMMAND testDbscTransaction)

# dbsc_account
add_library(dbscAccount)
target_sources(dbscAccount
  PRIVATE
    dbsc_account.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbsc_account.h
)
target_link_libraries(dbscAccount 
  PUBLIC 
    dbscTransaction
    dbscUuidString
    bdl
)
add_executable(testDbscAccount)
target_sources(testDbscAccount PRIVATE dbsc_account.t.cpp)
target_link_libraries(testDbscAccount PRIVATE dbscAccount)
add_test(NAME dbscAccount.t COMMAND testDbscAccount)

# dbsc_accountbook
add_library(dbscAccountBook)
target_sources(dbscAccountBook 
  PRIVATE
    dbsc_accountbook.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbsc_accountbook.h
)
target_include_directories(dbscAccountBook PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(dbscAccountBook 
  PUBLIC
    dbscAccount
    dbscUuidString
)

add_executable(testDbscAccountBook)
target_sources(testDbscAccountBook PRIVATE dbsc_accountbook.t.cpp)
target_link_libraries(testDbscAccountBook PRIVATE
  dbscAccountBook
  dbscAccount
  dbscUuidString
)
add_test(NAME dbscAccountBook.t COMMAND testDbscAccountBook)

# dbsc_dbsserializer
## Ensure the header compiles in isolation.
add_library(dbscSerializer)
target_sources(dbscSerializer
  PRIVATE
    dbsc_dbsserializer.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbsc_dbsserializer.h
)
target_link_libraries(dbscSerializer PUBLIC dbscAccountBook)

# dbsc_dbstomlserializer
add_library(dbscTomlSerializer) 
target_sources(dbscTomlSerializer 
  PRIVATE 
    dbsc_dbstomlserializer.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbsc_dbstomlserializer.h
)
target_link_libraries(dbscTomlSerializer PUBLIC
  dbscAccountBook
  dbscAccount
  dbscSerializer
  dbscTransaction
  dbscUuidString
  bdl
  bsl
  tomlplusplus::tomlplusplus
)

add_executable(testDbscTomlSerializer)
target_sources(testDbscTomlSerializer PRIVATE dbsc_dbstomlserializer.t.cpp)
target_link_libraries(testDbscTomlSerializer 
  PRIVATE 
    dbscTomlSerializer
    dbscAccountBook
    dbscTransaction
    dbscAccount
)
add_test(NAME dbscTomlSerializer.t COMMAND testDbscTomlSerializer)
