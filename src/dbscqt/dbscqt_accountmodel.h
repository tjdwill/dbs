// dbscqt_accountmodel.h
#ifndef INCLUDED_DBSCQT_ACCOUNTMODEL
#define INCLUDED_DBSCQT_ACCOUNTMODEL

//@PURPOSE: Provide a Qt representation for dbsc::Account
//
//@CLASSES:
//  dbscqt::AccountModel: Adheres to the Qt Model/View interface.
//  dbscqt::TransactionItem: Represents data for a single transaction.
//  dbscqt::ItemSortOrder: Determines how items are sorted.
//  dbscqt::TransactionItemData: A struct that packs needed information for the transaction.
//  dbscqt::AccountModelData: Information associated with the given account.
//
//@DESCRIPTION: This component contains the implementation of the Model/View paradigm for
//  the dbsc::Account class.

#include <QAbstractTableModel>
#include <QDateTime>
#include <QModelIndex>
#include <QString>
#include <QUuid>
#include <QVariant>

#include <memory>
#include <vector>

class QUuid;
class QDateTime;

namespace dbscqt {

struct TransactionItemData
{
  QDateTime mTimeStamp {};
  QString mTransactionAmount {};
  QString mNotes {};
  QString mOtherPartyAccountName {};
  QUuid mTransactionId {};
  QUuid mOtherPartyId {};
};

/// TODO: Define this class's interface.
/// TODO: Implement sorting (this should be a later feature)
/// Represents a row in the view. Its data is the data associated with a single
/// transaction.
class TransactionItem
{
public:
  /// @note @param transactionAmount is a QString due to its representation in dbsc
  /// (bdlfp::Decimal64). A double is not a good representation.
  TransactionItem( TransactionItemData const& transactionData );

  [[nodiscard]] inline auto amount() const -> QString const& { return mData.mTransactionAmount; }

  [[nodiscard]] inline auto notes() const -> QString const& { return mData.mNotes; }

  [[nodiscard]] inline auto otherPartyId() const -> QUuid { return mData.mOtherPartyId; }

  [[nodiscard]] inline auto timeStamp() const -> QDateTime const& { return mData.mTimeStamp; }

  [[nodiscard]] inline auto transactionId() const -> QUuid { return mData.mTransactionId; }

  /// @return the display name associated with the other party id
  /// @note format: `${AccountName} (<stringified first byte sequence of Uuid>)`
  [[nodiscard]] auto otherPartyDisplayName() const -> QString;

private:
  TransactionItemData mData;
};

struct AccountModelData
{
  QString mName;
  QString mDescription;
  QString mBalance;
  QUuid mId;
  bool mIsOpen;
};

/// An implementation of the QAbstractTableModel for a dbsc::Account.
/// Given the header inclusion order issue between Qt and BDE, no dbsc code can actually be
/// included, so AccountModel doesn't actually take any dbsc objects directly.
class AccountModel : public QAbstractTableModel
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
  /// model
  void addTransactionItem( std::unique_ptr< TransactionItem > );

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

/// @return Formatted string for displaying shortened uuid + name.
auto createDisplayText( QUuid id, QString const& name ) -> QString;
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
