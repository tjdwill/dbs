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
  auto otherPartyId() const -> std::string const&;
  auto timeStamp() const -> TimeStamp;
  auto transactionId() const -> std::string const&;

private:
  std::string mTransactionId;
  std::string mOtherPartyId;
  std::string mNotes {};
  BloombergLP::bdldfp::Decimal64 mAmount;
  TimeStamp mTimeStamp;
};
} // namespace dbsc
#endif // include guard
