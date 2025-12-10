// dbsc_accountbook.cpp
#include "dbsc_accountbook.h"

#include <dbsc_transaction.h>

#include <bdldfp_decimal.h>

#include <chrono>
#include <format>
#include <stdexcept>

namespace dbsc {

AccountBook::AccountBook( std::string const& ownerName )
  : mOwner( ownerName )
{
}

auto AccountBook::account( UuidString const& accountId ) const -> Account const&
{
  try {
    return mAccountsMap.at( accountId );
  } catch ( std::out_of_range const& ) {
    throw NonExistentAccountException( std::format( "Account with id {} does not exist.", accountId.view() ) );
  }
}

auto AccountBook::accountMut( UuidString const& accountId ) -> Account&
{
  try {
    return mAccountsMap.at( accountId );
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
  return mAccountsMap.begin();
}

auto AccountBook::begin() const -> const_iterator
{
  return mAccountsMap.begin();
}

auto AccountBook::cbegin() const noexcept -> const_iterator
{
  return mAccountsMap.cbegin();
}

auto AccountBook::end() -> iterator
{
  return mAccountsMap.end();
}

auto AccountBook::end() const -> const_iterator
{
  return mAccountsMap.end();
}

auto AccountBook::cend() const noexcept -> const_iterator
{
  return mAccountsMap.cend();
}

auto AccountBook::accountCount() const -> int
{
  return static_cast< int >( mAccountsMap.size() );
}

auto AccountBook::createAccount( std::string const& accountName, std::string const& accountDescription ) -> UuidString
{
  // This function must never fail, so continously generate ids until a unique
  // one is found.
  bool accountAdded { false };
  UuidString accountId;

  while ( not accountAdded ) {
    Account newAccount { accountName, accountDescription };
    if ( not mAccountsMap.contains( newAccount.id() ) ) {
      accountId                           = newAccount.id();
      auto const [_, insertionSuccessful] = mAccountsMap.insert( { accountId, std::move( newAccount ) } );
      BSLS_ASSERT( insertionSuccessful );
      accountAdded = true;
    }
  }

  return accountId;
}

auto AccountBook::makeTransaction(
  BloombergLP::bdldfp::Decimal64 amount,
  std::string const& notes,
  UuidString const& firstPartyId,
  std::optional< std::reference_wrapper< UuidString const > > internalSecondPartyIdOpt ) -> UuidString
{

  bool const internalSecondPartyIsPresent = internalSecondPartyIdOpt.has_value();
  if ( not account( firstPartyId ).isActive()
       || ( internalSecondPartyIsPresent && not account( internalSecondPartyIdOpt.value() ).isActive() ) ) {
    throw InactiveAccountException( "Attempted to make a transaction on an inactive account." );
  }

  auto generateId = [&firstPartyId, &internalSecondPartyIdOpt, internalSecondPartyIsPresent, this]() {
    bool idGenerated { false };
    UuidString generatedId;
    while ( not idGenerated ) {
      generatedId = UuidStringUtil::generate();
      if ( account( firstPartyId ).contains( generatedId )
           || ( internalSecondPartyIsPresent
                && account( internalSecondPartyIdOpt.value() ).contains( generatedId ) ) ) {
        // no-opt; continue loop
      } else {
        idGenerated = true;
      }
    }

    return generatedId;
  };

  UuidString const transactionId = generateId();
  TimeStamp const timeStamp      = std::chrono::system_clock::now();
  UuidString const secondPartyId = internalSecondPartyIdOpt ? *internalSecondPartyIdOpt : UuidString();

  accountMut( firstPartyId ).logTransaction( { transactionId, firstPartyId, secondPartyId, amount, timeStamp, notes } );
  if ( internalSecondPartyIsPresent ) {
    accountMut( internalSecondPartyIdOpt.value() )
      .logTransaction( { transactionId, internalSecondPartyIdOpt.value(), firstPartyId, -amount, timeStamp, notes } );
  }
  return transactionId;
}

void AccountBook::deactivate( UuidString const& accountId )
{
  accountMut( accountId ).deactivate();
}

void AccountBook::activate( UuidString const& accountId )
{
  accountMut( accountId ).activate();
}

void AccountBook::addParsedAccount( Account account )
{
  auto const accountId = account.id();
  try {
    mAccountsMap.insert( { accountId, std::move( account ) } );
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
