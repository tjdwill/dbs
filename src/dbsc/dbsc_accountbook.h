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

#include <dbsc_account.h>
#include <dbsc_registerexception.h>
#include <dbsc_sharedapi.h>
#include <dbsc_uuidstring.h>

#include <bdldfp_decimal.fwd.h>

#include <functional>
#include <map>
#include <string>

namespace dbsc {

class UuidString;
class Account;

DBSC_REGISTER_EXCEPTION( NonExistentAccountException, "" );

/// A collection of Accounts for a given user. This class allows for iteration
/// over its Accounts by way of key-value pairs [AccountId, Account]. It is
/// also responsible for recording transactions and can open new accounts as
/// well as toggle the status of an existing status (active/closed).
class DBSC_API AccountBook

{
public:
  using const_iterator =                                      // NOLINT
    std::map< UuidString, Account >::const_iterator;          // NOLINT
  using iterator = std::map< UuidString, Account >::iterator; // NOLINT

  AccountBook( std::string const& ownerName );

  // Accessors

  /// @return the account referred to by the identifier string.
  /// @throw @c dbsc::NonExistentAccount if the account does not exist.
  [[nodiscard]] auto account( UuidString const& accountId ) const -> Account const&;

  [[nodiscard]] auto owner() const -> std::string const&;

  [[nodiscard]] auto begin() -> iterator;
  [[nodiscard]] auto begin() const -> const_iterator;
  [[nodiscard]] auto cbegin() const noexcept -> const_iterator;
  [[nodiscard]] auto end() -> iterator;
  [[nodiscard]] auto end() const -> const_iterator;
  [[nodiscard]] auto cend() const noexcept -> const_iterator;

  [[nodiscard]] auto accountCount() const -> int;
  // Manipulators

  /// Create a new account based on the provided information, returning the
  /// account's Id.
  auto createAccount( std::string const& accountName, std::string const& description ) -> UuidString;

  /// @brief Record the transaction for the provided accountId and amounts.
  ///
  /// The @p internalSecondPartyIdOpt parameter is optional to allow transactions from
  /// external sources. As a result,
  /// it is assumed that if @p firstPartyId and @p internalSecondPartyIdOpt are valid Ids, they
  /// correspond to accounts that currently exist in the account book. If this is not the case,
  /// @c dbsc::NonExistentAccountException is thrown. If either account is inactive,
  /// @c dbsc::InactiveAccountException is thrown.
  ///
  /// To represent a deposit into @p firstPartyId, @p amount should be positive.
  /// Negative values represent withdrawals. If @p internalSecondPartyIdOpt is non-null, both
  /// accounts will share the same TimeStamp and transaction Id for their
  /// respective copies.
  ///
  /// @return the transaction id
  auto makeTransaction( BloombergLP::bdldfp::Decimal64 amount,
                        std::string const& transactionNotes,
                        UuidString const& firstPartyId,
                        std::optional< std::reference_wrapper< UuidString const > > internalSecondPartyIdOpt )
    -> UuidString;

  /// Modify the writability of a given account.
  /// @throw @c dbsc::NonExistentAccount if account does not exist.
  void deactivate( UuidString const& accountId );
  void activate( UuidString const& accountId );

  /// Insert the account into the collection.
  /// @note This function is intended for de-serialization purposes.
  void addParsedAccount( Account account );

private:
  auto accountMut( UuidString const& accountId ) -> Account&;

  std::string mOwner {};
  std::map< UuidString, Account > mAccountsMap {};
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
