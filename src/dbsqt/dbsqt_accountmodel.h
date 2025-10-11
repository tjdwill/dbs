// dbsqt_accountmodel.h
#ifndef INCLUDED_DBSQT_ACCOUNTMODEL
#define INCLUDED_DBSQT_ACCOUNTMODEL

//@PURPOSE: Provide a Qt representation for dbsc::Account
//
//@CLASSES:
//  dbsqt::AccountModel: Adheres to the Qt Model/View interface.
//  dbsqt::TransactionItem: Represents data for a single transaction.
//  dbsqt::ItemSortOrder: Determines how items are sorted.
//
//@DESCRIPTION: This component contains the implementation of the Model/View paradigm for
//  the dbsc::Account class.

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVariant>

#include <memory>
#include <vector>

namespace dbsqt {

/// TODO: Define this class's interface.
class TransactionItem;

enum class ItemSortOrder
{
  kAscending,  // Smallest to Largest (ex. 1, 2, 3)
  kDescending, // Largest to Smallest (ex. 3, 2, 1)
};

/// An implementation of the QAbstractTableModel for a dbsc::Account.
/// Given the header inclusion order issue between Qt and BDE, no dbsc code can actually be
/// included, so AccountModel doesn't actually take any dbsc objects directly.
class AccountModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  /// @param accountToModel is internally sorted by date.
  AccountModel( std::vector< std::unique_ptr< TransactionItem > > const& accountToModel );
  ~AccountModel() override;

  AccountModel( AccountModel const& )                    = delete;
  auto operator=( AccountModel const& ) -> AccountModel& = delete;
  AccountModel( AccountModel&& )                         = delete;
  auto operator=( AccountModel&& ) -> AccountModel&      = delete;

  auto rowCount( QModelIndex const& parent = QModelIndex() ) const -> int final;
  auto columnCount( QModelIndex const& parent = QModelIndex() ) const -> int final;
  auto data( QModelIndex const& index, int role = Qt::DisplayRole ) const -> QVariant final;
  auto headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const -> QVariant final;

public Q_SLOTS:
  /// Implement sorting by a given column
  void sortBy( int column, ItemSortOrder sortOrder = ItemSortOrder::kDescending );

private:
  class Private;
  std::unique_ptr< Private > mImp;
};
} // namespace dbsqt
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
