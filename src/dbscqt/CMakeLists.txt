add_compile_definitions(QT_NO_KEYWORDS)

# dbscqt_qobjectdeleteutil
add_library(dbscqt_qobjectdeleteutil)
target_sources(dbscqt_qobjectdeleteutil
  PRIVATE
    dbscqt_qobjectdeleteutil.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbscqt_qobjectdeleteutil.h
)
target_link_libraries(dbscqt_qobjectdeleteutil
  PUBLIC
    Qt::Core
    Qt::Widgets
)

add_executable(dbscqt_qobjectdeleteutil.t dbscqt_qobjectdeleteutil.t.cpp)
target_link_libraries(dbscqt_qobjectdeleteutil.t PRIVATE dbscqt_qobjectdeleteutil)
add_test(NAME dbscqtQObjectDeleteUtilTest COMMAND dbscqt_qobjectdeleteutil.t)

# dbscqt_accountmodel
add_library(dbscqt_accountmodel dbscqt_accountmodel.cpp)
target_sources(dbscqt_accountmodel 
  PRIVATE
    dbscqt_accountmodel.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbscqt_accountmodel.h
)
target_link_libraries(dbscqt_accountmodel PUBLIC 
  dbscAccount
  dbscTransaction
  dbsutl_helpers
  Qt::Core
  Qt::Gui
  Qt::Widgets
) 

add_executable(dbscqt_accountmodel.t dbscqt_accountmodel.t.cpp)
target_sources(dbscqt_accountmodel.t PRIVATE dbscqt_accountmodel.t.cpp)
target_compile_definitions(dbscqt_accountmodel.t PRIVATE PROJECT_RESOURCES_DIR="${PROJECT_SOURCE_DIR}/resources")
target_link_libraries(dbscqt_accountmodel.t 
  PRIVATE
    dbscqt_accountmodel
    dbscAccount
    dbscAccountBook
    dbscTransaction
    dbscTomlSerializer
    Qt::Core
    Qt::Widgets
)


# dbscqt_accountbookwidget
add_library(dbscqt_accountbookwidget)
target_sources(dbscqt_accountbookwidget
  PRIVATE
    dbscqt_accountbookwidget.cpp
    dbscqt_accountbookwidget.ui
  PUBLIC
    FILE_SET HEADERS
    FILES 
      dbscqt_accountbookwidget.h
)
target_link_libraries(dbscqt_accountbookwidget
  PRIVATE
    dbscqt_qobjectdeleteutil
    dbscqt_accountmodel
    dbscTransaction
  PUBLIC
    dbscAccountBook
    dbscAccount
    Qt::Widgets
)

