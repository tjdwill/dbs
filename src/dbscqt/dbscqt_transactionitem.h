// dbscqt_transactionitem.h
#ifndef INCLUDED_DBSCQT_TRANSACTIONITEM
#define INCLUDED_DBSCQT_TRANSACTIONITEM

//@PURPOSE: Create a Qt-compatible representation of a dbsc::Transaction for use in
//  Model/View programming
//
//@CLASSES:
//  dbscqt::TransactionItem: Represents data for a single transaction.
//  dbscqt::TransactionItemData: POD data for the trasnaction item.
//
//@DESCRIPTION: This component defines a means to represent displayable transaction data.

#include <QDateTime>
#include <QString>
#include <QUuid>

namespace dbsc {
class Account;
class AccountBook;
class Transaction;
} // namespace dbsc

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

/// Represents a row in the view. Its data is the data associated with a single
/// transaction.
class TransactionItem
{
public:
  /// @note @param transactionAmount is a QString due to its representation in dbsc
  /// (bdlfp::Decimal64). A double is not a good representation.
  explicit TransactionItem( TransactionItemData const& transactionData );
  ~TransactionItem();
  TransactionItem( TransactionItem const& )                    = delete;
  TransactionItem( TransactionItem&& )                         = delete;
  auto operator=( TransactionItem const& ) -> TransactionItem& = delete;
  auto operator=( TransactionItem&& ) -> TransactionItem&      = delete;

  [[nodiscard]] auto amount() const -> QString const&;
  [[nodiscard]] auto notes() const -> QString const&;
  [[nodiscard]] auto otherPartyId() const -> QUuid;
  [[nodiscard]] auto timeStamp() const -> QDateTime const&;
  [[nodiscard]] auto transactionId() const -> QUuid;
  /// @return the display name associated with the other party id
  /// @note format: `${AccountName} (<stringified first byte sequence of Uuid>)`
  [[nodiscard]] auto otherPartyDisplayName() const -> QString;

private:
  class Private;
  std::unique_ptr< Private > mImp;
};

[[nodiscard]] auto createTransactionItemData( dbsc::Transaction const& transaction,
                                              dbsc::AccountBook const& accountBook ) -> dbscqt::TransactionItemData;
/// @return a sequence of transaction items sorted in descending date order.
[[nodiscard]] auto createTransactionItems( dbsc::Account const& account, dbsc::AccountBook const& accountBook )
  -> std::vector< std::unique_ptr< dbscqt::TransactionItem > >;
} // namespace dbscqt
#endif

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
