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

#include <bdldfp_decimal.h>

#include <chrono>
#include <string>

namespace dbsc {
class Transaction
{
public:
private:
  std::string mTransactionId;
  std::string mOtherPartyId;
  std::string mNotes {};
  BloombergLP::bdldfp::Decimal64 mAmount;
  std::chrono::time_point<std::chrono::system_clock> mDateTime;
};
} // namespace dbsc
#endif // include guard
