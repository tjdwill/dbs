// dbscqt_transactionitem.h
#ifndef INCLUDED_DBSCQT_TRANSACTIONITEM
#define INCLUDED_DBSCQT_TRANSACTIONITEM
//@PURPOSE: Create a Qt-compatible representation of a dbsc::Transaction for use in
//  Model/View programming
//
//@CLASSES:
//  dbscqt::TransactionItem

#include <QDateTime>
#include <QString>
#include <QUuid>

namespace dbsc {
class Account;
class AccountBook;
} // namespace dbsc

namespace dbscqt {
struct TransactionItemData;

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

/// @return a sequence of transaction items sorted in descending date order.
auto createTransactionItems( dbsc::Account const& account, dbsc::AccountBook const& accountBook )
  -> std::vector< std::unique_ptr< dbscqt::TransactionItem > >;
} // namespace dbscqt
#endif
