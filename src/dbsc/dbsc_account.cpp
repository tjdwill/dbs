#include "dbsc_account.h"

#include <dbsc_transaction.h>
#include <dbsc_uuidstring.h>

#include <bdldfp_decimal.h>
#include <bsls_assert.h>

#include <algorithm>
#include <format>
#include <ranges>

namespace dbsc {

namespace {

  // Output transaction log in descending order. Use a reference wrapper to prevent
  // unnecessary copies of the entire transaction log. The referenced object lives
  // long enough for the reference to remain valid.
  struct DescendingTransactionSorter
  {
    auto operator()( Transaction const& a, Transaction const& b ) -> bool { return a.timestamp() > b.timestamp(); }
  };

  struct AscendingTransactionSorter
  {
    auto operator()( Transaction const& a, Transaction const& b ) -> bool { return a.timestamp() < b.timestamp(); }
  };
} // namespace

Account::Account( UuidString const& accountId, std::string const& name, std::string const& description )
  : mId( accountId )
  , mName( name )
  , mDescription( description )
{
  using namespace BloombergLP::bdldfp::DecimalLiterals;
  BSLS_ASSERT( mBalance == "0.0"_d64 );
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

auto Account::isActive() const -> bool
{
  return mIsActive;
}

auto Account::begin() -> MutableIterator
{
  return mTransactions.begin();
}

auto Account::begin() const -> ConstIterator
{
  return mTransactions.begin();
}

auto Account::cbegin() const noexcept -> ConstIterator
{
  return mTransactions.cbegin();
}

auto Account::end() -> MutableIterator
{
  return mTransactions.end();
}

auto Account::end() const -> ConstIterator
{
  return mTransactions.end();
}

auto Account::cend() const noexcept -> ConstIterator
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

void Account::logTransaction( Transaction transaction )
{
  UuidString const transactionId = transaction.transactionId();

  try {
    mTransactions.insert( { transactionId, std::move( transaction ) } );
  } catch ( std::out_of_range& /*error*/ ) {
    throw DuplicateUuidException( std::format( "Transaction {0} already exists.", transactionId.view() ) );
  }
  mBalance += mTransactions.at( transactionId ).amount();
}

void Account::deactivate()
{
  mIsActive = false;
}

void Account::activate()
{
  mIsActive = true;
}

//---

auto dbsc::AccountUtils::transactionsSorted( Account const& account, TransactionSorter sorter )
  -> std::vector< dbsc::AccountUtils::BorrowedTransaction >
{
  auto transactionsVector = account //
                          | std::views::transform( []( auto&& keyValPair ) { return std::cref( keyValPair.second ); } )
                          | std::ranges::to< std::vector >();
  std::ranges::sort( transactionsVector, sorter );

  return transactionsVector;
}

auto dbsc::AccountUtils::transactionsSortedByDescendingTimestamps( Account const& account )
  -> std::vector< dbsc::AccountUtils::BorrowedTransaction >
{
  return transactionsSorted( account, dbsc::DescendingTransactionSorter() );
}

auto dbsc::AccountUtils::transactionsSortedByAscendingTimestamps( Account const& account )
  -> std::vector< dbsc::AccountUtils::BorrowedTransaction >
{
  return transactionsSorted( account, dbsc::AscendingTransactionSorter() );
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
