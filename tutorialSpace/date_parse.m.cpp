// date_parse.m.cpp
// This program serves as practice for extracting a date from the chrono
// library, serializing it, and then deserializing it into a storable
// representation.

#include <cassert>
#include <chrono>
#include <format>
#include <iostream>
#include <sstream>
#include <string_view>

int main()
{
  using namespace std::string_view_literals;
  auto datetimeFormat = "%F %T"sv; // Date and Time
  auto now            = std::chrono::system_clock::now();
  std::stringstream ss {};
  ss << std::format("{0:%F} {0:%T}", now);
  // Alternatively, ss << now;
  std::cout << ss.str() << "\n";
  std::chrono::time_point<std::chrono::system_clock> parsed;

  std::chrono::from_stream(ss, datetimeFormat.data(), parsed);
  assert(parsed == now);
}
