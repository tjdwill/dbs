// dbscqt_qobjectdeleteutil.t.cpp
#include <dbscqt_qobjectdeleteutil.h>

#include <bsls_assert.h>

#include <QObject>
#include <QPointer>
#include <QScopedPointer>

namespace {
struct DbscqtQObjectDeleteTestSuite
{
  struct OrphanDeleteTest
  {
    OrphanDeleteTest()
    {
      auto someParent     = QScopedPointer< QObject >( new QObject() );
      auto orphanedObject = QPointer< QObject >( new QObject );
      auto childObject    = QPointer< QObject >( new QObject( someParent.get() ) );

      BSLS_ASSERT( orphanedObject && childObject && someParent );
      dbscqt::QObjectDeleteUtil::deleteOrphaned( childObject );
      dbscqt::QObjectDeleteUtil::deleteOrphaned( orphanedObject );
      BSLS_ASSERT( !orphanedObject && childObject );

      // Ensure nullptr delete doesn't crash
      dbscqt::QObjectDeleteUtil::deleteOrphaned( nullptr );
    }
  };

  struct UnconditionalDeleteTest
  {
    UnconditionalDeleteTest()
    {
      auto orphanedObject = QPointer< QObject >( new QObject() );
      auto childObject    = QPointer< QObject >( new QObject() );

      BSLS_ASSERT( orphanedObject && childObject );
      dbscqt::QObjectDeleteUtil::deleteUnchecked( childObject );
      dbscqt::QObjectDeleteUtil::deleteUnchecked( orphanedObject );
      BSLS_ASSERT( !orphanedObject && !childObject );
      //
      // Ensure nullptr delete doesn't crash
      dbscqt::QObjectDeleteUtil::deleteUnchecked( nullptr );
    }
  };

  DbscqtQObjectDeleteTestSuite()
  {
    OrphanDeleteTest();
    UnconditionalDeleteTest();
  }
};
} // namespace

int main()
{
  DbscqtQObjectDeleteTestSuite();

  return 0;
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
