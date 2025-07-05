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
#include <dbsc_uuid_string.h>

#include <bdldfp_decimal.h>

#include <map>
#include <string>

namespace dbsc {

class Account
{
public:
  [[nodiscard]] explicit Account(std::string const& name,
                                 std::string const& description);

  // Accessors
  [[nodiscard]] auto transaction(UuidString const& transactionId) const
    -> Transaction const&;

  // Manipulators
  void logTransaction(Transaction tranaction);

private:
  UuidString mId;
  std::string mName {};
  std::string mDescription {};
  BloombergLP::bdldfp::Decimal64 mBalance {};
  std::map<UuidString, Transaction> mTransactions {};
};

} // namespace dbsc
#endif // include guard
