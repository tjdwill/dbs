add_compile_definitions(QT_NO_KEYWORDS)
set(DBS_RESOURCES_DIR "${PROJECT_SOURCE_DIR}/resources") # Used for CMake
set(DBS_RESOURCES_DIR_MACRO "\"${PROJECT_SOURCE_DIR}/resources\"") # Used for C++ programs

# -- Helpers

set(qtResourcesFile "${DBS_RESOURCES_DIR}/resources.qrc")

# Add target to list of targets that need to be linked via windeployqt 
macro(register_deployable_target target_name)
  list(APPEND dbscqt_deployable_targets ${target_name})
endmacro()

function(retrieve_target_artifacts targetList oList)
  foreach(target IN LISTS targetList)
    list(APPEND targets "$<TARGET_FILE:${target}>")
    #message("${targets}")
  endforeach()
  set(${oList} "${targets}" PARENT_SCOPE)
endfunction()
# -- Targets

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
target_link_libraries(dbscqt_qobjectdeleteutil.t PRIVATE dbscqt_qobjectdeleteutil bsl)
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
target_link_libraries(dbscqt_displayutil 
  PUBLIC
    dbsc_transaction
    dbsc_uuidstring
    bdl
    Qt::Core
)
add_executable(dbscqt_displayutil.t dbscqt_displayutil.t.cpp)
register_deployable_target(dbscqt_displayutil.t)
target_link_libraries(dbscqt_displayutil.t PRIVATE dbscqt_displayutil bdl bsl Qt::Core)
add_test(NAME DbscqtDisplayUtilTest COMMAND dbscqt_displayutil.t)


add_library(dbscqt_transactiondialog)
target_sources(dbscqt_transactiondialog
  PRIVATE
    dbscqt_transactiondialog.cpp
  PUBLIC
    dbscqt_transactiondialog.ui
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbscqt_transactiondialog.h
)
target_link_libraries(dbscqt_transactiondialog
  PUBLIC
    Qt::Core
    Qt::Widgets
)
add_executable(dbscqt_transactiondialog.m dbscqt_transactiondialog.m.cpp)
register_deployable_target(dbscqt_transactiondialog.m)
target_link_libraries(dbscqt_transactiondialog.m 
  PRIVATE
    Qt::Core
    Qt::Widgets
    dbscqt_transactiondialog
    dbscqt_displayutil  
)


add_library(dbscqt_openaccountdialog)
target_sources(dbscqt_openaccountdialog
  PRIVATE
    dbscqt_openaccountdialog.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES
      dbscqt_openaccountdialog.h
      dbscqt_openaccountdialog.ui
)
target_link_libraries(dbscqt_openaccountdialog
  PUBLIC
    Qt::Core
    Qt::Widgets
)


add_library(dbscqt_transactionitem)
target_sources(dbscqt_transactionitem
  PRIVATE 
    dbscqt_transactionitem.cpp
  PUBLIC
    ${qtResourcesFile}
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
    bsl
  PUBLIC 
    Qt::Core
    Qt::Gui
    Qt::Widgets
) 
add_executable(dbscqt_accountmodel.m dbscqt_accountmodel.m.cpp)
register_deployable_target(dbscqt_accountmodel.m)
target_sources(dbscqt_accountmodel.m PRIVATE dbscqt_accountmodel.m.cpp)
target_compile_definitions(dbscqt_accountmodel.m PRIVATE DBS_RESOURCES_DIR=${DBS_RESOURCES_DIR_MACRO})
target_link_libraries(dbscqt_accountmodel.m 
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
  dbscqt_displayutil
  dbscqt_qobjectdeleteutil
  dbsc_account
  dbsc_accountbook
  dbsc_transaction
  bsl
 PUBLIC
  Qt::Widgets
  dbscqt_transactionitem
)
add_executable(dbscqt_accountbooktreewidget.m dbscqt_accountbooktreewidget.m.cpp)
register_deployable_target(dbscqt_accountbooktreewidget.m)
target_link_libraries(dbscqt_accountbooktreewidget.m
  PRIVATE
    dbscqt_accountbooktreewidget
    dbscqt_accountmodel
    dbscqt_displayutil
    dbscqt_transactionitem
    dbsc_accountbook
    dbsc_account
    bdl
    bsl
    Qt::Core
    Qt::Gui
    Qt::Widgets
)


add_library(dbscqt_accountbookwidget)
target_sources(dbscqt_accountbookwidget
  PRIVATE
    dbscqt_accountbookwidget.cpp
    dbscqt_accountbookwidget.ui
  PUBLIC
    ${qtResourcesFile}
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
    dbscqt_openaccountdialog
    dbscqt_qobjectdeleteutil
    dbscqt_transactiondialog
    dbsc_account
    dbsc_accountbook
    dbsc_transaction
    bsl
  PUBLIC
    dbscqt_transactionitem
    Qt::Widgets
)
add_executable(dbscqt_accountbookwidget.m)
register_deployable_target(dbscqt_accountbookwidget.m)
target_sources(dbscqt_accountbookwidget.m PRIVATE dbscqt_accountbookwidget.m.cpp)
target_link_libraries(dbscqt_accountbookwidget.m 
  PRIVATE
    dbscqt_accountbookwidget
    dbsc_tomlserializer
)
target_compile_definitions(dbscqt_accountbookwidget.m PRIVATE DBS_RESOURCES_DIR=${DBS_RESOURCES_DIR_MACRO})


# Since we are building test programs, we need to link Qt in order to run the Executabls on Windows.
if (WIN32)
  set_target_properties(${dbscqt_deployable_targets}
    PROPERTIES WIN32_EXECUTABLE TRUE
  )
  retrieve_target_artifacts("${dbscqt_deployable_targets}" windeployqt_args)

  set(deploy_command_args ${WINDEPLOYQT_EXECUTABLE}) 
  list(APPEND deploy_command_args "${windeployqt_args}")
  list(JOIN deploy_command_args " " deploy_command)
  # message(COMMAND: "${deploy_command}")
  add_custom_target(deployQtExecutables ALL
    COMMAND powershell "${deploy_command}" # Use powershell because the default cmd.exe doesn't handle pathing well
    COMMENT "Deploying Qt Executables."
    VERBATIM
  )
  add_dependencies(deployQtExecutables ${deployable_targets})
endif()
