
include(GenerateExportHeader)
add_library(dbsc)
generate_export_header(dbsc
  EXPORT_FILE_NAME dbsc_sharedapi.h
  EXPORT_MACRO_NAME DBSC_API
  NO_EXPORT_MACRO_NAME DBSC_INTERNAL
)
target_sources(dbsc
  PRIVATE
    dbsc_uuidstring.cpp
    dbsc_transaction.cpp
    dbsc_account.cpp
    dbsc_accountbook.cpp
    dbsc_dbscserializer.cpp
    dbsc_tomlserializer.cpp
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
    FILES
      dbsc_registerexception.h
      dbsc_uuidstring.h
      dbsc_transaction.h
      dbsc_account.h
      dbsc_accountbook.h
      dbsc_dbscserializer.h
      dbsc_tomlserializer.h
)
target_link_libraries(dbsc
  PUBLIC
    bdl
    bsl
    stduuid
    tomlplusplus::tomlplusplus
)

add_executable(dbsc_uuidstring.t)
target_sources(dbsc_uuidstring.t PRIVATE dbsc_uuidstring.t.cpp)
target_link_libraries(dbsc_uuidstring.t PRIVATE dbsc bsl)
add_test(NAME DbscUuidStringTest COMMAND dbsc_uuidstring.t)

add_executable(dbsc_transaction.t)
target_sources(dbsc_transaction.t PRIVATE dbsc_transaction.t.cpp)
target_link_libraries(dbsc_transaction.t PRIVATE dbsc bdl bsl)
add_test(NAME DbscTransactionTest COMMAND dbsc_transaction.t)

add_executable(dbsc_account.t)
target_sources(dbsc_account.t PRIVATE dbsc_account.t.cpp)
target_link_libraries(dbsc_account.t PRIVATE dbsc)
add_test(NAME DbscAccountTest COMMAND dbsc_account.t)


add_executable(dbsc_accountbook.t)
target_sources(dbsc_accountbook.t PRIVATE dbsc_accountbook.t.cpp)
target_link_libraries(dbsc_accountbook.t PRIVATE
  dbsc
  bsl
)
add_test(NAME DbscAccountBookTest COMMAND dbsc_accountbook.t)

add_executable(dbsc_tomlserializer.t)
target_sources(dbsc_tomlserializer.t PRIVATE dbsc_tomlserializer.t.cpp)
target_link_libraries(dbsc_tomlserializer.t 
  PRIVATE 
    dbsc
    bsl
)
add_test(NAME DbscTomlSerializerTest COMMAND dbsc_tomlserializer.t)
