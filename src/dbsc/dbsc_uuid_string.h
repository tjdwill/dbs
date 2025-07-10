// dbsc_uuid_string.h
#ifndef INCLUDED_DBSC_UUID_STRING
#define INCLUDED_DBSC_UUID_STRING

//@PURPOSE: Provide a wrapper around a string that enforces UUID format.
//
//@CLASSES:
//  dbsc::InvalidUuidException: communicate that a non-uuid-conforming string
//    was provided.
//  dbsc::UuidString: a non-modifiable string representing a valid RFC4122 UUID.
//  dbsc::UuidStringFactory: an interface to create UuidStrings.
//
//@DESCRIPTION: This component defines a wrapper to a string that ensures its
// RFC4122 compliance as a universal unique indentifier (UUID). Since it is
// intended to represent uniqueness, the string should be read-only, immune to
// outside (and possibly internal) modification.
//
/// Usage
/// -----
/// Example 1: Instantiation and Output
///
/// ```cpp
/// dbsc::UuidString randomId = dbsc::UuidStringUtil::generate();
/// assert(randomId != dbsc::UuidString());
/// assert(dbsc::UuidStringUtil::isNil(UuidString()));
/// assert(dbsc::UuidStringUtil::fromString(randomId.toStdString()) ==
///     randomId);
///
/// std::cout << randomId << "\n";
/// ```
///

#include <exception>
#include <iosfwd>
#include <string>
#include <string_view>

namespace dbsc {

class InvalidUuidException : std::exception
{
public:
  InvalidUuidException() noexcept;

  auto what() const noexcept -> char const* override;
};

/// This class provides a read-only UUID-compliant string. Note that this class
/// only guarantees UUID compliance in terms of RFC4122; this class makes no
/// guarantees that there are no duplicate UUIDs (for example, default
/// construction will always return the null UUID).
///
/// Note that this class assumes UUIDv4, but should work with others.
class UuidString
{
public:
  // Constructors enforce the invariant that the object represents a valid UUID.

  /// Returns the nil UUID "00000000-0000-0000-0000-000000000000"
  UuidString();

  [[nodiscard]] auto operator<=>(UuidString const& other) const = default;
  [[nodiscard]] auto operator==(UuidString const& other) const
    -> bool = default;
  [[nodiscard]] auto toStdString() const -> std::string;
  [[nodiscard]] auto view() const noexcept -> std::string_view;

private:
  friend struct UuidStringUtil;
  /// Construct a UuidString from an external string source.
  /// Private since this constructor assumes validation is performed before it's
  /// called.
  UuidString(std::string str);
  std::string mData {};
};

auto operator<<(std::ostream& oss, UuidString const& str) -> std::ostream&;

/// Interface for generating Uuids.
struct UuidStringUtil
{
  /// Attempt to construct a UuidString from an external source. Throws
  /// `dbsc::InvalidUuidException` if input is non-conformant.
  [[nodiscard]] static auto fromString(std::string candidate) -> UuidString;
  /// Generate a UUIDv4 (i.e. randomly-generated) string
  [[nodiscard]] static auto generate() -> UuidString;
  /// Queries if the uuid is the "00000000-0000-0000-0000-000000000000" string.
  [[nodiscard]] static auto isNil(UuidString const& uuid) -> bool;
};
} // namespace dbsc

#endif // include guard

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
