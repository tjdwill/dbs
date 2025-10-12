# dbsqt_accountmodel
add_library(dbsqtAccountModel dbsqt_accountmodel.cpp)
target_include_directories(dbsqtAccountModel PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/dbsc)
target_link_libraries(dbsqtAccountModel PUBLIC 
  dbscAccount
  dbscTransaction
  Qt::Core
  Qt::Gui
  Qt::Widgets
) 
add_executable(testDbsQtAccountModel dbsqt_accountmodel.t.cpp)
target_compile_definitions(testDbsQtAccountModel PRIVATE
  PROJECT_RESOURCES_DIR="${PROJECT_SOURCE_DIR}/resources")
target_include_directories(testDbsQtAccountModel PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/dbsc)
target_link_libraries(testDbsQtAccountModel PRIVATE
  dbsqtAccountModel
  dbscAccount
  dbscAccountBook
  dbscTransaction
  dbscTomlSerializer
  bsl
  bdl
  Qt::Core
  Qt::Gui
  Qt::Widgets)

