add_compile_definitions(QT_NO_KEYWORDS)
# dbscqt_accountmodel
add_library(dbscqtAccountModel dbscqt_accountmodel.cpp)
target_sources(dbscqtAccountModel 
  PRIVATE
    dbscqt_accountmodel.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbscqt_accountmodel.h
)
target_link_libraries(dbscqtAccountModel PUBLIC 
  dbscAccount
  dbscTransaction
  dbsutl_helpers
  Qt::Core
  Qt::Gui
  Qt::Widgets
) 

add_executable(testdbscqtAccountModel dbscqt_accountmodel.t.cpp)
target_sources(testdbscqtAccountModel PRIVATE dbscqt_accountmodel.t.cpp)
target_compile_definitions(testdbscqtAccountModel PRIVATE PROJECT_RESOURCES_DIR="${PROJECT_SOURCE_DIR}/resources")
target_link_libraries(testdbscqtAccountModel 
  PRIVATE
    dbscqtAccountModel
    dbscAccount
    dbscAccountBook
    dbscTransaction
    dbscTomlSerializer
    Qt::Core
    Qt::Widgets
)

