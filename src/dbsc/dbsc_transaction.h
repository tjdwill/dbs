// dbsc_transaction.h
#ifndef INCLUDED_DBSC_TRANSACTION
#define INCLUDED_DBSC_TRANSACTION

//@PURPOSE: Representation for DBS transactions.
//
//@CLASSES:
//  dbsc::Transaction: in-memory representation of an account transaction.
//
//@DESCRIPTION:
//
//  - Unique Transaction ID
//  - Other Party's ID
//  - Amount ($)
//  - DateTime
//  - Extra Notes
//

#include <bdldfp_decimal.fwd.h>
#include <bdldfp_decimal.h>

#include <chrono>
#include <string>

namespace dbsc {
using TimeStamp = std::chrono::time_point<std::chrono::system_clock>;
// So that we aren't confined to a specific uuid library, we use strings for the
// interface. However, these strings are generated internally, and no one can
// set an ID once the containing object is instantiated, so
using UuidString = std::string;

class Transaction
{
public:
  Transaction(std::string const& transactionId,
              std::string const& otherPartyID,
              BloombergLP::bdldfp::Decimal64 amount,
              TimeStamp timeStamp,
              std::string const& notes);

  auto amount() const -> BloombergLP::bdldfp::Decimal64;
  auto notes() const -> std::string const&;
  auto otherPartyId() const -> UuidString const&;
  auto timeStamp() const -> TimeStamp;
  auto transactionId() const -> UuidString const&;

private:
  UuidString mTransactionId;
  UuidString mOtherPartyId;
  std::string mNotes {};
  BloombergLP::bdldfp::Decimal64 mAmount;
  TimeStamp mTimeStamp;
};
} // namespace dbsc
#endif // include guard
