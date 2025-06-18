// hello.cpp
#include <format>
#include <iostream>
#include <string_view>
#include <toml++/toml.hpp>

using namespace std::string_view_literals;

int main()
{
  auto name        = "tjdwill"sv;
  toml::table test = toml::parse(std::format(R"(
multi_string = """

This is a raw string that is

intended to represent a TOML value.
"""

name = "{}"  # This means we can actually substitute variables into TOML data via C++ code.
  )"sv,
                                             name));
  std::cout << test["multi_string"sv] << "\n";
  std::cout << test << "\n";
}

// -----------------------------------------------------------------------------
// Copyright (C) 2025 Terrance Williams
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// ------------------------------ END_OF_FILE ----------------------------------
