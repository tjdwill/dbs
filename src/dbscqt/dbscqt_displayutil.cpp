// dbscqt_displayutil.cpp
#include "dbscqt_displayutil.h"

auto dbscqt::DisplayUtil::accountNameWithShortenedUuid( QUuid id, QString const& name ) -> QString
{
  return QString( "%1 (%2)" ).arg( name ).arg( id.toString( QUuid::WithoutBraces ).split( '-' ).front() );
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
