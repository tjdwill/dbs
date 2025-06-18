add_executable(hello hello.m.cpp)
target_include_directories(hello
  PRIVATE $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/3rdparty/tomlplusplus/include>
)
