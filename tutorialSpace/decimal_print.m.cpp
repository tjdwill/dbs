// decimal_print.m.cpp
// A program to test if bdldfp::Decimal classes can be printed with STL
// facilities.
#include <bdldfp_decimal.h>

#include <cassert>
#include <iostream>
#include <ranges>
#include <vector>

using namespace BloombergLP;
using namespace bdldfp::DecimalLiterals;

int main()
{
  bdldfp::Decimal64 test {};
  assert( test == "0"_d64 );

  std::vector< bdldfp::Decimal64 > v;
  for (int i = 0; i < 10; ++i) {
    v.push_back( "0.12345"_d64 );
  }

  for (auto const& [idx, num] : std::views::enumerate( v )) {
    std::cout << num * idx << " ";
  }
  std::cout << "\n";
}
