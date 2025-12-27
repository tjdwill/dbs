// dbscqt_accountmodel.h
#ifndef INCLUDED_DBSCQT_ACCOUNTMODEL
#define INCLUDED_DBSCQT_ACCOUNTMODEL

//@PURPOSE: Provide a Qt representation for dbsc::Account
//
//@CLASSES:
//  dbscqt::AccountModel: Adheres to the Qt Model/View interface.
//
//@DESCRIPTION: This component contains the implementation of the Model/View paradigm for
//  the dbsc::Account class.

#include <dbscqt_sharedapi.h>

#include <QtCore/QAbstractTableModel>
#include <QtCore/QDateTime>
#include <QtCore/QModelIndex>
#include <QtCore/QString>
#include <QtCore/QUuid>
#include <QtCore/QVariant>

#include <memory>
#include <vector>

class QUuid;
class QDateTime;

namespace dbscqt {

class TransactionItem;

/// An implementation of the QAbstractTableModel for a dbsc::Account.
class DBSCQT_API AccountModel final : public QAbstractTableModel
{
  Q_OBJECT

public:
  /// @param accountToModel is internally sorted by date.
  AccountModel( std::vector< std::unique_ptr< TransactionItem > > transactionItems, QObject* parent = nullptr );
  ~AccountModel() override;

  [[nodiscard]] auto rowCount( QModelIndex const& parent = QModelIndex() ) const -> int final;
  [[nodiscard]] auto columnCount( QModelIndex const& parent = QModelIndex() ) const -> int final;
  [[nodiscard]] auto data( QModelIndex const& index, int role = Qt::DisplayRole ) const -> QVariant final;
  [[nodiscard]] auto headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const
    -> QVariant final;

  /// @note Assumes validation has been done on this transaction before adding to the
  /// model. Takes ownership of the incoming transaction item.
  void addTransactionItem( std::unique_ptr< TransactionItem > transactionItemPtr );

public Q_SLOTS:
  //  /// Implement sorting by a given column
  //  void sortBy( int column, Qt::SortOrder sortOrder = Qt::DescendingOrder );

private:
  AccountModel( AccountModel const& )                    = delete;
  auto operator=( AccountModel const& ) -> AccountModel& = delete;
  AccountModel( AccountModel&& )                         = delete;
  auto operator=( AccountModel&& ) -> AccountModel&      = delete;

  class Private;
  std::unique_ptr< Private > mImp;
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
