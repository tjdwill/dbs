// dbsutl_helpers.t.cpp
#include <dbsutl_helpers.h>

#include <cassert>

struct EnumAsIntegralTestSuite
{
  struct IntegerTest
  {
    IntegerTest()
    {
      constexpr int kManualOffset = 457;
      enum class TestEnum
      {
        kAlpha,
        kBravo,
        kCharlie = kManualOffset,
      };
      static_assert( dbsutl::enumAsIntegral( TestEnum::kAlpha ) == 0 );
      static_assert( dbsutl::enumAsIntegral( TestEnum::kBravo ) == 1 );
      static_assert( dbsutl::enumAsIntegral( TestEnum::kCharlie ) == kManualOffset );
    };
  };

  // run tests in suite
  EnumAsIntegralTestSuite() { IntegerTest {}; }
};

int main()
{
  EnumAsIntegralTestSuite();

  return 0;
}
