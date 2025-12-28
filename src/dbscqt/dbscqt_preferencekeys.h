// dbscqt_preferencekeys.h
#ifndef INCLUDED_DBSCQT_PREFERENCEKEYS
#define INCLUDED_DBSCQT_PREFERENCEKEYS

//@PURPOSE: Define keys used for application settings
//
//@CLASSES:
//  dbscqt::PreferenceKeys: A struct of inline setting key constants.
//
//@DESCRIPTION: This component defines a struct that contains application
//  QSettings keys.

#include <QtCore/QString>

namespace dbscqt {

struct PreferenceKeys
{
  /// Used to detect if the program shutdown via crash.
  inline static QString const kAccountBookIsCurrentlyModified { "state/AccountBookUpToDate" };
  inline static QString const kLastAccountBookFileDirectoryKey { "state/LastAccountBookFileDirectory" };
  inline static QString const kRecentAccountBookPathKey { "state/recentAccountBook" };

  // MainWindow settings
  inline static QString const kWindowGeometryKey { "window/Geometry" };
  inline static QString const kWindowStateKey { "window/State" };

  inline static QString const kApplicationColorSchemeKey { "colorScheme" };
};
} // namespace dbscqt

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
