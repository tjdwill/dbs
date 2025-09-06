// dbsc_accountbook.h
#ifndef INCLUDED_DBSC_ACCOUNTBOOK
#define INCLUDED_DBSC_ACCOUNTBOOK
//@PURPOSE: Provide a container for multiple accounts belonging to a user.
//
//@CLASSES:
//  dbsc::AccountBook: a collection of multiple dbsc::Accounts.
//  dbsc::NonExistingAccountException: an error that signals that the queried
//  account does not exist.
//
//@DESCRIPTION: This component defines a collection of accounts. The primary
//  purpose is to allow iteration over a single user's accounts.

#include <dbsc/dbsc_account.h>
#include <dbsc/dbsc_uuidstring.h>

#include <bdldfp_decimal.fwd.h>

#include <exception>
#include <functional>
#include <map>
#include <optional>
#include <string>

namespace dbsc {

class UuidString;
class Account;

class NonExistentAccountException : public std::exception
{
public:
  NonExistentAccountException( std::string const& errorMessage ) noexcept;
  NonExistentAccountException() noexcept;

  auto what() const noexcept -> char const* override;

private:
  std::string mErrorMsg {};
};

/// A collection of Accounts for a given user. This class allows for iteration
/// over its Accounts by way of key-value pairs [AccountId, Account]. It is
/// also responsible for recording transactions and can open new accounts as
/// well as toggle the status of an existing status (opened/closed).
class AccountBook

{
public:
  using const_iterator =                                      // NOLINT
    std::map< UuidString, Account >::const_iterator;          // NOLINT
  using iterator = std::map< UuidString, Account >::iterator; // NOLINT

  AccountBook( std::string const& ownerName );

  // Accessors

  /// Return the account referred to by the identifier string.
  /// If the account does not exist, this function throws the
  /// dbsc::NonExistentAccount error.
  [[nodiscard]] auto account( UuidString const& accountId ) const -> Account const&;
  [[nodiscard]] auto owner() const -> std::string const&;

  [[nodiscard]] auto begin() const -> const_iterator;
  [[nodiscard]] auto cbegin() const noexcept -> const_iterator;
  [[nodiscard]] auto end() const -> const_iterator;
  [[nodiscard]] auto cend() const noexcept -> const_iterator;

  [[nodiscard]] auto accountCount() const -> int;
  // Manipulators

  /// Create a new account based on the provided information, returning the
  /// account's Id.
  auto createAccount( std::string const& accountName, std::string const& description ) -> UuidString;

  /// Record the transaction for the provided accountId and amounts. Returns the
  /// transaction Id.
  /// The @p otherPartyId parameter is optional to allow transactions from
  /// external sources without needing to check for the nil UUID. As a result,
  /// it is assumed that @p accountId and @p otherPartyId are both valid Ids
  /// that currently exist in the account book. If this is not the case,
  /// dbsc::NonExistentAccount is thrown.
  ///
  /// To represent a deposit into @p accountId, @p amount should be positive.
  /// Negative values represent withdrawls. If @p otherPartyId is provided, both
  /// accounts will share the same TimeStamp and transaction Id for their
  /// respective copies.
  auto makeTransaction( BloombergLP::bdldfp::Decimal64 amount,
                        std::string const& transactionNotes,
                        UuidString const& accountId,
                        std::optional< std::reference_wrapper< UuidString const > > otherPartyId = std::nullopt )
    -> UuidString;

  /// Modify the writability of a given account.
  /// Throws dbsc::NonExistentAccount if account does not exist.
  void closeAccount( UuidString const& accountId );
  void openAccount( UuidString const& accountId );

  /// Insert the account into the collection. This function is intended for
  /// de-serialization purposes.
  void addParsedAccount( Account account );

private:
  auto accountMut( UuidString const& accountId ) -> Account&;
  auto begin() -> iterator;
  auto end() -> iterator;

  std::string mOwner {};
  std::map< UuidString, Account > mAccounts {};
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
