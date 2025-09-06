// dbsc_accountbook.cpp
#include "dbsc_accountbook.h"

#include <dbsc/dbsc_transaction.h>

#include <bdldfp_decimal.h>

#include <chrono>
#include <format>
#include <stdexcept>

namespace dbsc {

NonExistentAccountException::NonExistentAccountException( std::string const& errorMessage ) noexcept
  : mErrorMsg( errorMessage )
{
}

NonExistentAccountException::NonExistentAccountException() noexcept
  : NonExistentAccountException( "" ) {};

auto NonExistentAccountException::what() const noexcept -> char const*
{
  return mErrorMsg.c_str();
}

AccountBook::AccountBook( std::string const& ownerName )
  : mOwner( ownerName )
{
}

auto AccountBook::account( UuidString const& accountId ) const -> Account const&
{
  try {
    return mAccounts.at( accountId );
  } catch ( std::out_of_range const& ) {
    throw NonExistentAccountException( std::format( "Account with id {} does not exist.", accountId.view() ) );
  }
}

auto AccountBook::accountMut( UuidString const& accountId ) -> Account&
{
  try {
    return mAccounts.at( accountId );
  } catch ( std::out_of_range const& ) {
    throw NonExistentAccountException( std::format( "Account with id {} does not exist.", accountId.view() ) );
  }
}

auto AccountBook::owner() const -> std::string const&
{
  return mOwner;
}

auto AccountBook::begin() -> iterator
{
  return mAccounts.begin();
}

auto AccountBook::begin() const -> const_iterator
{
  return mAccounts.begin();
}

auto AccountBook::cbegin() const noexcept -> const_iterator
{
  return mAccounts.cbegin();
}

auto AccountBook::end() -> iterator
{
  return mAccounts.end();
}

auto AccountBook::end() const -> const_iterator
{
  return mAccounts.end();
}

auto AccountBook::cend() const noexcept -> const_iterator
{
  return mAccounts.cend();
}

auto AccountBook::accountCount() const -> int
{
  return static_cast< int >( mAccounts.size() );
}

auto AccountBook::createAccount( std::string const& accountName, std::string const& accountDescription ) -> UuidString
{
  // This function shouldn't fail, so attempt to add a new account
  // in the extremely unlikely event of duplicate id.
  bool accountAdded { false };
  UuidString accountId;

  while ( not accountAdded ) {
    Account newAccount { accountName, accountDescription };
    if ( mAccounts.contains( newAccount.id() ) ) {
      continue;
    }
    accountId = newAccount.id();
    mAccounts.insert( { accountId, std::move( newAccount ) } );

    accountAdded = true;
  }

  return accountId;
}

auto AccountBook::makeTransaction( BloombergLP::bdldfp::Decimal64 amount,
                                   std::string const& notes,
                                   UuidString const& accountId,
                                   std::optional< std::reference_wrapper< UuidString const > > otherPartyId )
  -> UuidString
{

  if ( !account( accountId ).isOpen() || ( otherPartyId && !account( *otherPartyId ).isOpen() ) ) {
    throw ClosedAccountException( "Attempted to make a transaction on a closed account." );
  }

  auto loopGenerateId = [&accountId, &otherPartyId, this]() {
    bool idGenerated { false };
    UuidString generatedId;
    while ( not idGenerated ) {
      generatedId = UuidStringUtil::generate();
      if ( account( accountId ).contains( generatedId )
           || ( otherPartyId.has_value() && account( *otherPartyId ).contains( generatedId ) ) ) {
      } else {
        idGenerated = true;
      }
    }

    return generatedId;
  };

  UuidString const transactionId = loopGenerateId();
  TimeStamp const timeStamp      = std::chrono::system_clock::now();
  UuidString const secondPartyId = otherPartyId ? *otherPartyId : UuidString();

  accountMut( accountId ).logTransaction( { transactionId, accountId, secondPartyId, amount, timeStamp, notes } );
  if ( otherPartyId ) {
    accountMut( *otherPartyId )
      .logTransaction( { transactionId, *otherPartyId, accountId, -amount, timeStamp, notes } );
  }
  return transactionId;
}

void AccountBook::closeAccount( UuidString const& accountId )
{
  accountMut( accountId ).closeAccount();
}

void AccountBook::openAccount( UuidString const& accountId )
{
  accountMut( accountId ).openAccount();
}

void AccountBook::addParsedAccount( Account account )
{
  auto const accountId = account.id();
  try {
    mAccounts.insert( { accountId, std::move( account ) } );
  } catch ( std::out_of_range const& ) {
    throw DuplicateUuidException( std::format(
      "Account with id {} already exists with name '{}'.", accountId.view(), this->account( accountId ).name() ) );
  }
}

} // namespace dbsc

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
