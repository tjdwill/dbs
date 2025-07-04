// dbsc_account.h
#ifndef INCLUDED_DBSC_ACCOUNT
#define INCLUDED_DBSC_ACCOUNT

// The representation of accounts for the application. Accounts have the
// following pieces of data:
//     - Account ID (uuid)
//     - Account Name
//     - Account Description
//     - Balance
//     - Transaction Record

#include <dbsc_transaction.h>

#include <bdldfp_decimal.h>

#include <string>
#include <vector>

namespace dbsc {

class Account
{
public:
private:
  std::string mId;
  std::string mName {};
  std::string mDescription {};
  BloombergLP::bdldfp::Decimal64 mBalance {};
  std::vector<Transaction> mTransactions {}; // change to map?
};

} // namespace dbsc
#endif // include guard
