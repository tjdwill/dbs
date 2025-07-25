// dbsc_accountbook.h
#ifndef INCLUDED_DBSC_ACCOUNTBOOK
#define INCLUDED_DBSC_ACCOUNTBOOK
//@PURPOSE: Provide a container for multiple accounts belonging to a user.
//
//@CLASSES:
//  dbsc::AccountBook: a collection of multiple dbsc::Accounts.
//
//@DESCRIPTION: This component defines a collection of accounts. The primary
//  purpose is to allow iteration over a single user's accounts.

#include <map>
#include <optional>
#include <string>

namespace dbsc {

class UuidString;
class Account;

/// A collection of Accounts for a given user. This class allows for iteration
/// over its Accounts by way of key-value pairs [AccountId, Account].
class AccountBook
{
public:
  AccountBook( std::string const& ownerName );

  /// Return the account referred to by the identifier string.
  /// If the account does not exist,
  [[nodiscard]] auto account( UuidString const& accountId ) const
    -> std::optional< std::reference_wrapper< Account > >;
  [[nodiscard]] auto owner() const -> std::string const&;

  /// Store an Account in the account book.
  /// If an account with the identifier already exists,
  /// this function will throw dbsc::DuplicateUuidException.
  auto openAccount( Account const& account ) -> UuidString;

private:
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
