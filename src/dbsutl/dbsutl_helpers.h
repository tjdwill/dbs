// dbsutl_helpers.h

#ifndef INCLUDED_DBSUTL_HELPERS
#define INCLUDED_DBSUTL_HELPERS

//@PURPOSE: Provide helper functions that facilitate code implementation.
//
//@DESCRIPTION: This component serves as a housing for useful functions.
//  At the time of writing, there is no defined structure due to a lack of quantity,
//  but note that functions may be to be moved around over time.

#include <concepts>
#include <map>
#include <stdexcept>

namespace dbsutl {

template< typename EnumType, std::integral ReturnType = int >
constexpr auto enumAsIntegral( EnumType type ) -> ReturnType
{
  return static_cast< ReturnType >( type );
}

/// Given an associative map, @return a map with the keys and values reversed.
/// This assumes that every key in the input map has a unique value such that
/// the resulting two maps are bi-directional.
///
/// @throws @c std::invalid_argument if the input map is not bijective
/// (one-to-one).
template< typename KeyType, typename ValueType >
auto createReversedMap( std::map< KeyType, ValueType > const map ) -> std::map< ValueType, KeyType >
{
  std::map< ValueType, KeyType > reversedMap;
  for ( auto const& [key, value] : map ) {
    reversedMap.insert( { value, key } );
  }

  if ( reversedMap.size() != map.size() ) {
    throw std::invalid_argument( "Could not create reversed map due to non-unique values." );
  }

  return reversedMap;
}

} // namespace dbsutl

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
