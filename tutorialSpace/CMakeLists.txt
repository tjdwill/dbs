set(DBS_THIRDPARTY_BASE_DIR ${PROJECT_SOURCE_DIR}/3rdparty)
add_executable(hello)
target_sources(hello 
  PRIVATE
    hello.m.cpp
  PRIVATE
    FILE_SET helloHdrs
    TYPE HEADERS
    BASE_DIRS
      ${DBS_THIRDPARTY_BASE_DIR}/tomlplusplus/include 
    FILES
      ${DBS_THIRDPARTY_BASE_DIR}/tomlplusplus/include/toml++/toml.hpp
)

add_executable(uuidTest)
target_sources(uuidTest 
  PRIVATE
    uuid_test.m.cpp
  PRIVATE
    FILE_SET uuidTestHdrs
    TYPE HEADERS
    BASE_DIRS
      ${DBS_THIRDPARTY_BASE_DIR}
    FILES
      ${DBS_THIRDPARTY_BASE_DIR}/stduuid/uuid.h
)

add_executable(dateParseTest)
target_sources(dateParseTest
  PRIVATE
    date_parse.m.cpp
)

add_executable(bdlDecimal)
target_sources(bdlDecimal
  PRIVATE
    decimal_print.m.cpp
)
target_link_libraries(bdlDecimal PRIVATE bdl bsl)

add_executable(bdlDecimalQt)
target_sources(bdlDecimalQt
  PRIVATE
    decimalQt.m.cpp
)
target_link_libraries(bdlDecimalQt PRIVATE bdl bsl Qt::Core)

