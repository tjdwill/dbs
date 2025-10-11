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
