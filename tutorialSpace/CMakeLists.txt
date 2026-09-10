add_executable( hello hello.m.cpp )
target_link_libraries( hello PRIVATE tomlplusplus::tomlplusplus )

add_executable( uuidTest uuid_test.m.cpp )
target_link_libraries( uuidTest PRIVATE stduuid )

add_executable( dateParseTest date_parse.m.cpp )

add_executable( bdlDecimal decimal_print.m.cpp )
target_link_libraries( bdlDecimal PRIVATE bdl bsl )

add_executable( bdlDecimalQt decimalQt.m.cpp )
target_link_libraries( bdlDecimalQt PRIVATE bdl bsl Qt::Core )
target_compile_definitions( bdlDecimalQt PUBLIC QT_NO_KEYWORDS )
