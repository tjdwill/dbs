include_directories(.)

add_library(dbsTransaction dbsc_transaction.cpp)
target_link_libraries(dbsTransaction PUBLIC bdl)

add_library(dbsAccount dbsc_account.cpp)
target_link_libraries(dbsAccount PUBLIC 
    dbsTransaction
    bdl
)
