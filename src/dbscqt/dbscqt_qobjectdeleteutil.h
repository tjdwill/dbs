// dbscqt_qobjectdeleteutil.h
#ifndef INCLUDED_DBSCQT_QOBJECTDELETEUTIL
#define INCLUDED_DBSCQT_QOBJECTDELETEUTIL

//@PURPOSE: Define useful QObject deleter functions for convenience.
//
//@CLASSES:
//  dbscqt::QObjectDeleteUtil: A collection of deletion functions.
//  dbscqt::OrphanDeleterPrivate: Delete orphaned QObjects (QObjects with no parent).
//    Internal to this component
//  dbscqt::UnconditionalDeleterPrivate: Delete QObjects unconditionally. Internal to this
//    component.
//
//@DESCRIPTION: This component defines functions that facilitate conditional deletion
//  of QObjects.

#include <QtCore/QObject>
#include <QtCore/QPointer>

namespace dbscqt {
/// Deletes QObjects that are unparented.
struct OrphanDeleterPrivate
{
  void operator()( QObject* objectHandle )
  {
    if ( objectHandle && !objectHandle->parent() ) {
      delete objectHandle;
    };
  }
};

/// Always calls delete on the object handle (valid because delete on nullptr is a
/// no-op).
struct UnconditionalDeleterPrivate
{
  void operator()( QObject* objectHandle ) { delete objectHandle; };
};

struct QObjectDeleteUtil
{

  template< typename DeletionPolicy >
    requires std::invocable< DeletionPolicy, QObject* >
  static void deleteQObjectLike( QObject* handle, DeletionPolicy deleter )
  {
    deleter( handle );
  }

  /// Delete QObject that does not have a parent.
  static void deleteOrphaned( QObject* handle ) { deleteQObjectLike( handle, OrphanDeleterPrivate() ); }

  /// Unconditionally delete the QObject.
  static void deleteUnchecked( QObject* handle ) { deleteQObjectLike( handle, UnconditionalDeleterPrivate() ); }
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
