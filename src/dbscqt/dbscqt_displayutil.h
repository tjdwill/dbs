// dbscqt_displayutil.h
#ifndef INCLUDED_DBSCQT_DISPLAYUTIL
#define INCLUDED_DBSCQT_DISPLAYUTIL
//@PURPOSE: Provide types and/or factory methods for creation of Qt representations of dbsc
//   classes. Necessary for interfacing with Qt widgets.
//
//@CLASSES:
//  dbscqt::DisplayUtil: Stores the methods for facilitating the display of dbsc data.
//
//@DESCRIPTION: This component defines Qt-versions of dbsc types and functions to create
//  them.

#include <dbsc_uuidstring.h>

#include <QDateTime>
#include <QString>
#include <QUuid>

namespace dbscqt {
struct DisplayUtil
{
  /// @return Formatted string for displaying shortened uuid + name.
  [[nodiscard]] static auto accountNameWithShortenedUuid( QUuid id, QString const& name ) -> QString;
  [[nodiscard]] static auto toQUuid( dbsc::UuidString const& uuidString ) -> QUuid;
};
} // namespace dbscqt

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
