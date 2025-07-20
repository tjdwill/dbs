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
#include <dbsc_uuidstring.h>

#include <bdldfp_decimal.fwd.h>
#include <bdldfp_decimal.h>

#include <map>
#include <string>

namespace dbsc {

class Account
{
public:
  using const_iterator =                                          // NOLINT
    std::map< UuidString, Transaction >::const_iterator;          // NOLINT
  using iterator = std::map< UuidString, Transaction >::iterator; // NOLINT

  [[nodiscard]] explicit Account( std::string const& name,
                                  std::string const& description );

  // Accessors

  [[nodiscard]] auto balance() const -> BloombergLP::bdldfp::Decimal64;
  [[nodiscard]] auto description() const -> std::string const&;
  [[nodiscard]] auto id() const -> UuidString const&;
  [[nodiscard]] auto name() const -> std::string const&;
  auto begin() -> iterator;
  auto begin() const -> const_iterator;
  auto cbegin() const noexcept -> const_iterator;
  auto end() -> iterator;
  auto end() const -> const_iterator;
  auto cend() const noexcept -> const_iterator;

  /// Retrieve the transaction data associated with the input transactionID.
  /// Propagates the `std::out_of_range` error if the transaction doesn't exists
  /// for the account.
  [[nodiscard]] auto transaction( UuidString const& transactionId ) const
    -> Transaction const&;

  // Manipulators

  /// Store the provided transaction into the account's store.
  /// Throws `dbsc::DuplicateUuid` if the transaction is a duplicate.
  void logTransaction( Transaction transaction );

private:
  UuidString mId;
  std::string mName {};
  std::string mDescription {};
  BloombergLP::bdldfp::Decimal64 mBalance {};
  std::map< UuidString, Transaction > mTransactions {};
};

} // namespace dbsc
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
