add_compile_definitions(QT_NO_KEYWORDS)
# dbsqt_accountmodel
add_library(dbsqtAccountModel dbsqt_accountmodel.cpp)
target_sources(dbsqtAccountModel 
  PRIVATE
    dbsqt_accountmodel.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbsqt_accountmodel.h
)
target_link_libraries(dbsqtAccountModel PUBLIC 
  dbscAccount
  dbscTransaction
  dbsutl_helpers
  Qt::Core
  Qt::Gui
  Qt::Widgets
) 

add_executable(testDbsQtAccountModel dbsqt_accountmodel.t.cpp)
target_sources(testDbsQtAccountModel PRIVATE dbsqt_accountmodel.t.cpp)
target_compile_definitions(testDbsQtAccountModel PRIVATE PROJECT_RESOURCES_DIR="${PROJECT_SOURCE_DIR}/resources")
target_link_libraries(testDbsQtAccountModel 
  PRIVATE
    dbsqtAccountModel
    dbscAccount
    dbscAccountBook
    dbscTransaction
    dbscTomlSerializer
    bsl
    bdl
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

