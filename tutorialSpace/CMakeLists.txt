project( dbsTutorialSpace )

find_package( tomlplusplus REQUIRED )
find_package( stduuid REQUIRED )

# Bloomberg BDE imports
find_package( Threads REQUIRED )
find_package( bdl REQUIRED )
find_package( bsl REQUIRED )

find_package( Qt6 REQUIRED COMPONENTS Core Gui Widgets )
qt_standard_project_setup()

add_executable( hello hello.m.cpp )
target_link_libraries( hello PRIVATE tomlplusplus::tomlplusplus dbsCompilationConfigs )

add_executable( uuidTest uuid_test.m.cpp )
target_link_libraries( uuidTest PRIVATE stduuid dbsCompilationConfigs )

add_executable( dateParseTest date_parse.m.cpp )
target_link_libraries( dateParseTest PRIVATE dbsCompilationConfigs )

add_executable( bdlDecimal decimal_print.m.cpp )
target_link_libraries( bdlDecimal PRIVATE bdl bsl dbsCompilationConfigs )

add_executable( bdlDecimalQt decimalQt.m.cpp )
target_link_libraries( bdlDecimalQt PRIVATE bdl bsl Qt::Core dbsCompilationConfigs )
target_compile_definitions( bdlDecimalQt PUBLIC QT_NO_KEYWORDS )
