// dbsc_account.h
#ifndef INCLUDED_DBSC_ACCOUNT
#define INCLUDED_DBSC_ACCOUNT

//@PURPOSE: Provide a means of tracking transactions for a given purpose.
//
//@CLASSES:
//  dbsc::Account: an entity that tracks a monetary balance over multiple
//    transactions.
//  dbsc::AccountUtils: Stores free functions to extend Account.
//  dbsc::InactiveAccountException: an error that denotes an illegal transaction.
//
//@DESCRIPTION: This component defines the Account, which is essentially the
//  backbone of the budgeting system. Accounts store Transactions, keeps track
//  of balance, and stores user-defined metadata such as the account name and
//  description.

#include <dbsc_registerexception.h>
#include <dbsc_sharedapi.h>
#include <dbsc_transaction.h>
#include <dbsc_uuidstring.h>

#include <bdldfp_decimal.fwd.h>

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace dbsc {

/// Signals that a caller attempted to add a Transaction to an inactive Account.
DBSC_REGISTER_EXCEPTION( InactiveAccountException, "Attempted to modify an inactive account." );

/// The basis of the application, this class stores multiple transactions,
/// allowing one to track the balance of money at a given moment. Accounts have
/// two states, "active" and "inactive," which is the way this program disables
/// modifying the Account further without actually deleting the data (someone
/// may wish to reactivate at a future date).
class Account
{
public:
  using ConstIterator   = std::map< UuidString, Transaction >::const_iterator;
  using MutableIterator = std::map< UuidString, Transaction >::iterator;

  [[nodiscard]] DBSC_API explicit Account( UuidString const& accountId,
                                           std::string const& name,
                                           std::string const& description );
  [[nodiscard]] DBSC_API explicit Account( std::string const& name, std::string const& description );

  // Accessors

  [[nodiscard]] DBSC_API auto balance() const -> BloombergLP::bdldfp::Decimal64;
  [[nodiscard]] DBSC_API auto description() const -> std::string const&;
  [[nodiscard]] DBSC_API auto id() const -> UuidString const&;
  [[nodiscard]] DBSC_API auto name() const -> std::string const&;
  [[nodiscard]] DBSC_API auto transactionCount() const -> int;
  [[nodiscard]] DBSC_API auto begin() -> MutableIterator;
  [[nodiscard]] DBSC_API auto begin() const -> ConstIterator;
  [[nodiscard]] DBSC_API auto cbegin() const noexcept -> ConstIterator;
  [[nodiscard]] DBSC_API auto end() -> MutableIterator;
  [[nodiscard]] DBSC_API auto end() const -> ConstIterator;
  [[nodiscard]] DBSC_API auto cend() const noexcept -> ConstIterator;

  /// Query if this account has a transaction with the provided Id.
  [[nodiscard]] DBSC_API auto contains( UuidString const& transactionId ) const -> bool;

  /// Retrieve the transaction data associated with the input transactionID.
  /// Propagates the @c std::out_of_range error if the transaction doesn't exist
  /// for the account.
  [[nodiscard]] DBSC_API auto transaction( UuidString const& transactionId ) const -> Transaction const&;

  /// Queries if the account is open for making new transactions.
  [[nodiscard]] DBSC_API auto isActive() const -> bool;

  // Manipulators

  /// Store the provided transaction into the account's store.
  /// Throws `dbsc::DuplicateUuidException` if the transaction is a duplicate.
  /// Throws `dbsc::ClosedAccountException` if the account is closed at the time
  /// this function is called.
  DBSC_API void logTransaction( Transaction transaction );

  /// Sets this account's status to "read-only". No further transactions can be
  /// added.
  DBSC_API void deactivate();

  /// Sets this account's status to "writeable", meaning it can store new
  /// transactions.
  DBSC_API void activate();

  [[nodiscard]] DBSC_API friend auto operator==( Account const& a, Account const& b ) -> bool = default;

private:
  UuidString mId;
  std::string mName {};
  std::string mDescription {};
  std::map< UuidString, Transaction > mTransactions {};
  BloombergLP::bdldfp::Decimal64 mBalance {};
  bool mIsActive { true };
};

struct AccountUtils
{
  using BorrowedKeyValuePair =
    std::pair< std::reference_wrapper< UuidString const >, std::reference_wrapper< Transaction const > >;
  using TransactionSorter = std::function< bool( BorrowedKeyValuePair const&, BorrowedKeyValuePair const& ) >;

  /// @brief Retrieve a sequence of references to the transactions sorted by @a sorter.
  ///
  /// @warning The caller is responsible for ensuring the lifetime of the @a account is
  /// at least as long as the last point-of-use of the references.
  [[nodiscard]] static auto transactionsSorted( Account const& account, TransactionSorter sorter )
    -> std::vector< BorrowedKeyValuePair >;

  /// @brief Retrieve a sequence of references to the transactions sorted from most to least recent.
  ///
  /// @warning The caller is responsible for ensuring the lifetime of the @a account is
  /// at least as long as the last point-of-use of the references.
  [[nodiscard]] static auto transactionsSortedByDescendingTimestamps( Account const& account )
    -> std::vector< BorrowedKeyValuePair >;

  /// @brief Retrieve a sequence of references to the transactions sorted from least to most recent.
  ///
  /// @warning The caller is responsible for ensuring the lifetime of the @a account is
  /// at least as long as the last point-of-use of the references.
  [[nodiscard]] static auto transactionsSortedByAscendingTimestamps( Account const& account )
    -> std::vector< BorrowedKeyValuePair >;
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
