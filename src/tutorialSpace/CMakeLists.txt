add_executable(hello hello.m.cpp)
target_include_directories(hello
  PRIVATE $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/3rdparty/tomlplusplus/include>
)

add_executable(uuidTest uuid_test.m.cpp)
target_include_directories(uuidTest
  PRIVATE ${CMAKE_SOURCE_DIR}/3rdparty)

add_executable(dateParseTest date_parse.m.cpp)

