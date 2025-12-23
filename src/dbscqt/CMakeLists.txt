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

include(GenerateExportHeader)
add_library(dbscqt)
generate_export_header(dbscqt
  EXPORT_FILE_NAME dbscqt_sharedapi.h
  EXPORT_MACRO_NAME DBSCQT_API
  NO_EXPORT_MACRO_NAME DBSCQT_INTERNAL
)
target_sources(dbscqt
  PRIVATE
    dbscqt_accountbooktreewidget.cpp
    dbscqt_accountbookwidget.cpp
    dbscqt_accountmodel.cpp
    dbscqt_displayutil.cpp
    dbscqt_mainwindow.cpp
    dbscqt_openaccountdialog.cpp
    dbscqt_qobjectdeleteutil.cpp
    dbscqt_transactiondialog.cpp
    dbscqt_transactionitem.cpp
  PUBLIC
    ${qtResourcesFile}
    dbscqt_accountbookwidget.ui
    dbscqt_mainwindow.ui
    dbscqt_openaccountdialog.ui
    dbscqt_transactiondialog.ui
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
    FILES
      dbscqt_accountbooktreewidget.h
      dbscqt_accountbookwidget.h
      dbscqt_accountmodel.h
      dbscqt_displayutil.h
      dbscqt_mainwindow.h
      dbscqt_openaccountdialog.h
      dbscqt_qobjectdeleteutil.h
      dbscqt_transactiondialog.h
      dbscqt_transactionitem.h
      ${CMAKE_CURRENT_BINARY_DIR}/dbscqt_sharedapi.h
)
target_link_libraries(dbscqt 
  PRIVATE
    bsl
  PUBLIC
    bdl
    dbsc
    dbsutl
    Qt::Core
    Qt::Gui
    Qt::Widgets
)
install(TARGETS dbscqt)


add_executable(dbscqt_qobjectdeleteutil.t dbscqt_qobjectdeleteutil.t.cpp)
set(isLinux "$<PLATFORM_ID:Linux>")
set(usingClang "$<CXX_COMPILER_ID:Clang>")
set(isAsanEligible "$<AND:${isLinux},${usingClang}>")
target_compile_options(dbscqt_qobjectdeleteutil.t PRIVATE "$<${isAsanEligible}:-fsanitize=address;-fno-omit-frame-pointer>")
target_link_options(dbscqt_qobjectdeleteutil.t PRIVATE "$<${isAsanEligible}:-fsanitize=address;-fno-omit-frame-pointer>")
target_link_libraries(dbscqt_qobjectdeleteutil.t PRIVATE dbscqt bsl)
add_test(NAME DbscqtQObjectDeleteUtilTest COMMAND dbscqt_qobjectdeleteutil.t)


add_executable(dbscqt_displayutil.t dbscqt_displayutil.t.cpp)
register_deployable_target(dbscqt_displayutil.t)
target_link_libraries(dbscqt_displayutil.t PRIVATE dbscqt bdl bsl Qt::Core)
add_test(NAME DbscqtDisplayUtilTest COMMAND dbscqt_displayutil.t)


add_executable(dbscqt_transactiondialog.m dbscqt_transactiondialog.m.cpp)
register_deployable_target(dbscqt_transactiondialog.m)
target_link_libraries(dbscqt_transactiondialog.m 
  PRIVATE
    Qt::Core
    Qt::Widgets
    dbscqt
)

add_executable(dbscqt_accountmodel.m dbscqt_accountmodel.m.cpp)
register_deployable_target(dbscqt_accountmodel.m)
target_sources(dbscqt_accountmodel.m PRIVATE dbscqt_accountmodel.m.cpp)
target_compile_definitions(dbscqt_accountmodel.m PRIVATE DBS_RESOURCES_DIR=${DBS_RESOURCES_DIR_MACRO})
target_link_libraries(dbscqt_accountmodel.m 
  PRIVATE
    dbscqt
    dbsc
)


add_executable(dbscqt_accountbooktreewidget.m dbscqt_accountbooktreewidget.m.cpp)
register_deployable_target(dbscqt_accountbooktreewidget.m)
target_link_libraries(dbscqt_accountbooktreewidget.m
  PRIVATE
    dbscqt
    dbsc
    bdl
    bsl
    Qt::Core
    Qt::Gui
    Qt::Widgets
)


add_executable(dbscqt_accountbookwidget.m)
register_deployable_target(dbscqt_accountbookwidget.m)
target_sources(dbscqt_accountbookwidget.m PRIVATE dbscqt_accountbookwidget.m.cpp)
target_link_libraries(dbscqt_accountbookwidget.m 
  PRIVATE
    dbscqt
    dbsc
)
target_compile_definitions(dbscqt_accountbookwidget.m PRIVATE DBS_RESOURCES_DIR=${DBS_RESOURCES_DIR_MACRO})


add_executable(dbs)
register_deployable_target(dbs)
target_sources(dbs PRIVATE dbs.m.cpp)
target_link_libraries(dbs 
  PRIVATE
    dbscqt
    Qt::Core
    Qt::Widgets
)

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

include(GNUInstallDirs)
install(TARGETS dbs)
qt_generate_deploy_app_script(
  TARGET dbs
  OUTPUT_SCRIPT deploy_script
  NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})