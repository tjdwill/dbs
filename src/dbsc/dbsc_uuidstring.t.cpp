// dbsc_uuid_string.t.cpp
// Test Driver for UuidString and its factory
#include <dbsc/dbsc_uuidstring.h>

#include <cassert>
#include <iostream> // for testing
#include <ranges>   // for testing

namespace {
static void testException()
{
  try {
    auto const failedUuid = dbsc::UuidStringUtil::fromString( "Invalid UUID" );
  } catch ( dbsc::InvalidUuidException const& e ) {
  }
}

static void testGeneration()
{
  dbsc::UuidString nilUuid {};
  assert( dbsc::UuidStringUtil::isNil( nilUuid ) );

  // Test the generation functions as well as the conversions to and from
  // std::strings.
  auto base = dbsc::UuidStringUtil::generate();
  for ( auto _ : std::views::iota( 0, 1000 ) ) {
    auto generated = dbsc::UuidStringUtil::generate();
    std::cout << generated << "\n";
    assert( base != generated );
    assert( nilUuid != generated );
    assert( generated == dbsc::UuidStringUtil::fromString( std::string( generated.view() ) ) );
    assert( generated == dbsc::UuidStringUtil::fromString( generated.toStdString() ) );
  }
}
} // namespace

int main()
{
  testException();
  testGeneration();
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
