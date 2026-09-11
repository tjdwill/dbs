// dbsc_timestamp.h
#ifndef INCLUDED_DBSC_TIMESTAMP
#define INCLUDED_DBSC_TIMESTAMP

//@PURPOSE: Define type alias for use in multiple components
//
//@CLASSES:
//  dbsc::TimeStamp: Represents a point in time.
//
//@DESCRIPTION: This component defines a type alias for use in multiple components.

#include <chrono>
#include <cstdint>
#include <ratio>

namespace dbsc {
using TimeStamp =
  std::chrono::time_point< std::chrono::system_clock, std::chrono::duration< std::int64_t, std::nano > >;
} // namespace dbsc

#endif // header include guard

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
