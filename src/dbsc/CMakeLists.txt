include_directories(.)

set(debugOptions -Wall -Werror -Wextra -Wpedantic)

# dbsc_uuid_string
add_library(dbscUuidString dbsc_uuidstring.cpp)
target_compile_options(dbscUuidString PRIVATE $<$<CONFIG:Debug>:${debugOptions}>)
target_include_directories(dbscUuidString PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty)
add_executable(testDbscUuidString dbsc_uuidstring.t.cpp)
target_link_libraries(testDbscUuidString PRIVATE
  dbscUuidString)
add_test(NAME dbscUuidString.t COMMAND testDbscUuidString)

# dbsc_transaction
add_library(dbscTransaction dbsc_transaction.cpp)
target_link_libraries(dbscTransaction PUBLIC 
  dbscUuidString
  bdl)
add_executable(testDbscTransaction dbsc_transaction.t.cpp)
target_link_libraries(testDbscTransaction PRIVATE dbscTransaction bdl)
add_test(NAME dbscTransaction.t COMMAND testDbscTransaction)

# dbsc_account
add_library(dbscAccount dbsc_account.cpp)
target_link_libraries(dbscAccount PUBLIC 
    dbscTransaction
    dbscUuidString
    bdl)
add_executable(testDbscAccount dbsc_account.t.cpp)
target_link_libraries(testDbscAccount PRIVATE dbscAccount)
add_test(NAME dbscAccount.t COMMAND testDbscAccount)

# dbsc_accountbook
add_library(dbscAccountBook dbsc_accountbook.cpp)
target_link_libraries(dbscAccountBook PUBLIC
  dbscAccount
  dbscUuidString)
add_executable(testDbscAccountBook dbsc_accountbook.t.cpp)
target_link_libraries(testDbscAccountBook PRIVATE
  dbscAccountBook
  dbscAccount
  dbscUuidString)
add_test(NAME dbscAccountBook.t COMMAND testDbscAccountBook)


# dbsc_dbstomlserializer
add_library(dbscToml dbsc_dbstomlserializer.cpp)
target_link_libraries(dbscToml PUBLIC
  dbscAccountBook
  dbscAccount
  dbscTransaction
  dbscUuidString
  bdl
  bsl)
add_executable(testDbscToml dbsc_dbstomlserializer.t.cpp)
target_link_libraries(testDbscToml PRIVATE dbscToml dbscAccountBook dbscTransaction dbscAccount)
add_test(NAME dbscToml.t COMMAND testDbscToml)
