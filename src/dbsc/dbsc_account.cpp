#include "dbsc_account.h"

#include <dbsc/dbsc_transaction.h>
#include <dbsc/dbsc_uuidstring.h>

#include <bdldfp_decimal.h>

#include <cassert>
#include <format>

namespace dbsc {

ClosedAccountException::ClosedAccountException( std::string const& errorMessage ) noexcept
  : mErrorMsg( errorMessage )
{
}

ClosedAccountException::ClosedAccountException() noexcept
  : ClosedAccountException( "Attempted to modify a closed account." )
{
}

auto ClosedAccountException::what() const noexcept -> char const*
{
  return mErrorMsg.c_str();
}

Account::Account( UuidString const& accountId, std::string const& name, std::string const& description )
  : mId( accountId )
  , mName( name )
  , mDescription( description )
{
  using namespace BloombergLP::bdldfp::DecimalLiterals;
  assert( mBalance == "0.0"_d64 );
}

Account::Account( std::string const& name, std::string const& description )
  : Account( UuidStringUtil::generate(), name, description )
{
}

auto Account::balance() const -> BloombergLP::bdldfp::Decimal64
{
  return mBalance;
}

auto Account::description() const -> std::string const&
{
  return mDescription;
}

auto Account::id() const -> UuidString const&
{
  return mId;
}

auto Account::name() const -> std::string const&
{
  return mName;
}

auto Account::transactionCount() const -> int
{
  return static_cast< int >( mTransactions.size() );
}

auto Account::isOpen() const -> bool
{
  return mIsOpen;
}

auto Account::begin() -> iterator
{
  return mTransactions.begin();
}

auto Account::begin() const -> const_iterator
{
  return mTransactions.begin();
}

auto Account::cbegin() const noexcept -> const_iterator
{
  return mTransactions.cbegin();
}

auto Account::end() -> iterator
{
  return mTransactions.end();
}

auto Account::end() const -> const_iterator
{
  return mTransactions.end();
}

auto Account::cend() const noexcept -> const_iterator
{
  return mTransactions.cend();
}

auto Account::contains( UuidString const& transactionId ) const -> bool
{
  return mTransactions.contains( transactionId );
}

auto Account::transaction( UuidString const& transactionId ) const -> Transaction const&
{
  return mTransactions.at( transactionId );
}

void Account::logTransaction( Transaction const& transaction, UuidString const& transactionId )
{
  if ( not isOpen() ) {
    throw ClosedAccountException();
  }

  UuidString const transactionId = transaction.transactionId();

  try {
    mTransactions.insert( { transactionId, transaction } );
  } catch ( std::out_of_range& /*error*/ ) {
    throw DuplicateUuidException( std::format( "Transaction {0} already exists.", transactionId.view() ) );
  }
  mBalance += mTransactions.at( transactionId ).amount();
}

void Account::closeAccount()
{
  mIsOpen = false;
}

void Account::openAccount()
{
  mIsOpen = true;
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
