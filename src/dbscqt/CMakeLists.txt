add_compile_definitions(QT_NO_KEYWORDS)
set(DBS_RESOURCES_DIR "\"${PROJECT_SOURCE_DIR}/resources\"")

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
set(isLinux "$<PLATFORM_ID:Linux>")
set(usingClang "$<CXX_COMPILER_ID:Clang>")
set(isAsanEligible "$<AND:${isLinux},${usingClang}>")
target_compile_options(dbscqt_qobjectdeleteutil.t PRIVATE "$<${isAsanEligible}:-fsanitize=address;-fno-omit-frame-pointer>")
target_link_options(dbscqt_qobjectdeleteutil.t PRIVATE "$<${isAsanEligible}:-fsanitize=address;-fno-omit-frame-pointer>")
target_link_libraries(dbscqt_qobjectdeleteutil.t PRIVATE dbscqt_qobjectdeleteutil)
add_test(NAME DbscqtQObjectDeleteUtilTest COMMAND dbscqt_qobjectdeleteutil.t)

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
  dbsc_account
  dbsc_transaction
  dbsutl_helpers
  Qt::Core
  Qt::Gui
  Qt::Widgets
) 

add_executable(dbscqt_accountmodel.t dbscqt_accountmodel.t.cpp)
target_sources(dbscqt_accountmodel.t PRIVATE dbscqt_accountmodel.t.cpp)
target_compile_definitions(dbscqt_accountmodel.t PRIVATE DBS_RESOURCES_DIR=${DBS_RESOURCES_DIR})
target_link_libraries(dbscqt_accountmodel.t 
  PRIVATE
    dbscqt_accountmodel
    dbsc_account
    dbsc_accountbook
    dbsc_transaction
    dbsc_tomlserializer
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
    dbsc_transaction
  PUBLIC
    dbscqt_accountmodel
    dbsc_accountbook
    dbsc_account
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
