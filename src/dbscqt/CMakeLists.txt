add_compile_definitions(QT_NO_KEYWORDS)
set(DBS_RESOURCES_DIR "\"${PROJECT_SOURCE_DIR}/resources\"")


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
set(isLinux "$<PLATFORM_ID:Linux>")
set(usingClang "$<CXX_COMPILER_ID:Clang>")
set(isAsanEligible "$<AND:${isLinux},${usingClang}>")
target_compile_options(dbscqt_qobjectdeleteutil.t PRIVATE "$<${isAsanEligible}:-fsanitize=address;-fno-omit-frame-pointer>")
target_link_options(dbscqt_qobjectdeleteutil.t PRIVATE "$<${isAsanEligible}:-fsanitize=address;-fno-omit-frame-pointer>")
target_link_libraries(dbscqt_qobjectdeleteutil.t PRIVATE dbscqt_qobjectdeleteutil)
add_test(NAME DbscqtQObjectDeleteUtilTest COMMAND dbscqt_qobjectdeleteutil.t)

add_library(dbscqt_displayutil)
target_sources(dbscqt_displayutil
  PRIVATE
    dbscqt_displayutil.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbscqt_displayutil.h
)
target_link_libraries(dbscqt_displayutil PUBLIC Qt::Core)
add_executable(dbscqt_displayutil.t dbscqt_displayutil.t.cpp)
target_link_libraries(dbscqt_displayutil.t PRIVATE dbscqt_displayutil)
add_test(NAME DbscqtDisplayUtilTest COMMAND dbscqt_displayutil.t)

add_library(dbscqt_transactionitem)
target_sources(dbscqt_transactionitem
  PRIVATE 
    dbscqt_transactionitem.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbscqt_transactionitem.h
)
target_link_libraries(dbscqt_transactionitem 
  PRIVATE 
    dbscqt_displayutil 
    dbsc_account
    dbsc_accountbook
  PUBLIC 
    Qt::Core
)

add_library(dbscqt_accountmodel dbscqt_accountmodel.cpp)
target_sources(dbscqt_accountmodel 
  PRIVATE
    dbscqt_accountmodel.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbscqt_accountmodel.h
)
target_link_libraries(dbscqt_accountmodel 
  PRIVATE
    dbsutl_helpers
    dbscqt_transactionitem
  PUBLIC 
    Qt::Core
    Qt::Gui
    Qt::Widgets
) 
add_executable(dbscqt_accountmodel.t dbscqt_accountmodel.t.cpp)
target_sources(dbscqt_accountmodel.t PRIVATE dbscqt_accountmodel.t.cpp)
target_compile_definitions(dbscqt_accountmodel.t PRIVATE DBS_RESOURCES_DIR=${DBS_RESOURCES_DIR})
target_link_libraries(dbscqt_accountmodel.t 
  PRIVATE
    dbscqt_transactionitem
    dbscqt_accountmodel
    dbsc_account
    dbsc_accountbook
    dbsc_transaction
    dbsc_tomlserializer
)

add_library(dbscqt_accountbooktreewidget)
target_sources(dbscqt_accountbooktreewidget
  PRIVATE 
    dbscqt_accountbooktreewidget.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbscqt_accountbooktreewidget.h
)
target_link_libraries(dbscqt_accountbooktreewidget
 PRIVATE
  dbscqt_accountmodel
  dbsc_account
  dbsc_accountbook
 PUBLIC
  dbscqt_transactionitem
)

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
    dbscqt_accountbooktreewidget
    dbscqt_accountmodel
    dbscqt_displayutil
    dbscqt_qobjectdeleteutil
    dbscqt_transactionitem
    dbsc_account
    dbsc_accountbook
    dbsc_transaction
  PUBLIC
    Qt::Widgets
)
add_executable(dbscqt_accountbookwidget.t)
target_sources(dbscqt_accountbookwidget.t PRIVATE dbscqt_accountbookwidget.t.cpp)
target_link_libraries(dbscqt_accountbookwidget.t 
  PRIVATE
    dbscqt_accountbookwidget
    dbsc_tomlserializer
)
target_compile_definitions(dbscqt_accountbookwidget.t PRIVATE DBS_RESOURCES_DIR=${DBS_RESOURCES_DIR})
