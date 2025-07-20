// dbsc_uuid_string.cpp
#include "dbsc_uuidstring.h"

#include <stduuid/uuid.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <functional>
#include <random>

namespace dbsc {

DuplicateUuidException::DuplicateUuidException(
  std::string const& errorMessage ) noexcept
  : mErrorMsg( std::move( errorMessage ) )
{
}

DuplicateUuidException::DuplicateUuidException() noexcept
  : dbsc::DuplicateUuidException( "An object with this UUID already exists." )
{
}

auto DuplicateUuidException::what() const noexcept -> char const*
{
  return mErrorMsg.c_str();
}

InvalidUuidException::InvalidUuidException() noexcept {};

auto InvalidUuidException::what() const noexcept -> char const*
{
  return "Input is not a UUID-conformant string.";
}

UuidString::UuidString()
  : mData( uuids::to_string( uuids::uuid {} ) )
{
}

UuidString::UuidString( std::string str )
  : mData( std::move( str ) )
{
}

auto UuidString::toStdString() const -> std::string
{
  return mData;
}

auto UuidString::view() const noexcept -> std::string_view
{
  return mData;
}

auto UuidStringUtil::fromString( std::string candidate ) -> UuidString
{
  auto test = uuids::uuid::from_string( std::string_view( candidate ) );
  if (not test.has_value()) {
    throw InvalidUuidException {};
  }

  return UuidString( candidate );
}

auto UuidStringUtil::generate() -> UuidString
{
  static bool sGeneratorInitialized { false };
  static std::mt19937 sGenerator;

  if (not sGeneratorInitialized) {
    std::random_device rd {};
    auto seedData = std::array< int, std::mt19937::state_size > {};
    std::generate(
      std::begin( seedData ), std::end( seedData ), std::ref( rd ) );
    std::seed_seq seq( std::begin( seedData ), std::end( seedData ) );
    std::mt19937 generator { seq };
    sGeneratorInitialized = true;
  }
  static uuids::uuid_random_generator sUuidGenerator { sGenerator };

  return UuidString( uuids::to_string( sUuidGenerator() ) );
}

auto UuidStringUtil::isNil( UuidString const& uuid ) -> bool
{
  return uuids::uuid::from_string( uuid.view() ).value().is_nil();
}
} // namespace dbsc

auto dbsc::operator<<( std::ostream& os, dbsc::UuidString const& uuid )
  -> std::ostream&
{
  os << uuid.view();
  return os;
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
