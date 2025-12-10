// dbsc_accountbook.t.cpp
#include <dbsc_account.h>
#include <dbsc_accountbook.h>
#include <dbsc_uuidstring.h>

#include <bdldfp_decimal.h>
#include <bsls_assert.h>

#include <functional>
#include <string>
#include <string_view>

namespace {

using namespace BloombergLP::bdldfp::DecimalLiterals;
using namespace std::string_view_literals;
std::string_view constexpr kOwnerName { "tjdwill"sv };

} // namespace

int main()
{

  dbsc::AccountBook accountBook { std::string { kOwnerName } };

  // Test: adding accounts
  BSLS_ASSERT( accountBook.accountCount() == 0 );
  auto const accountId = accountBook.createAccount( "TestAccount", "Some Description" );
  BSLS_ASSERT( accountBook.accountCount() == 1 );
  accountBook.addParsedAccount( dbsc::Account( "SomeOwner", "" ) );
  BSLS_ASSERT( accountBook.accountCount() == 2 );

  // Test accessors
  try {
    auto const badAccountAccess = accountBook.account( {} );
    BSLS_ASSERT( false ); // Should never be reached.
  } catch ( dbsc::NonExistentAccountException const& ) {
  }

  BSLS_ASSERT( accountBook.owner() == kOwnerName );
  BSLS_ASSERT( accountBook.account( accountId ) == accountBook.account( accountId ) );
  BSLS_ASSERT( accountBook.cbegin() != accountBook.cend() );
  BSLS_ASSERT( accountBook.account( accountId ).isActive() );

  // Account operations
  accountBook.deactivate( accountId );
  BSLS_ASSERT( not accountBook.account( accountId ).isActive() );
  accountBook.activate( accountId );
  BSLS_ASSERT( accountBook.account( accountId ).isActive() );

  /// External deposit
  auto const kTransactionAmount = "100.00"_d64;
  accountBook.makeTransaction( kTransactionAmount, "transactionNotes", accountId, std::nullopt );
  BSLS_ASSERT( accountBook.account( accountId ).balance() == kTransactionAmount );

  /// Inter-acccount Transaction
  auto const secondAccountId = accountBook.createAccount( "SecondAccount", "Second Description" );

  auto const transactionId =
    accountBook.makeTransaction( kTransactionAmount, "Internal account transactions", secondAccountId, accountId );
  BSLS_ASSERT( accountBook.account( accountId ).balance() == BloombergLP::bdldfp::Decimal64() );
  BSLS_ASSERT( accountBook.account( secondAccountId ).balance() == kTransactionAmount );
  BSLS_ASSERT( dbsc::Transaction::isPair( accountBook.account( secondAccountId ).transaction( transactionId ),
                                          accountBook.account( accountId ).transaction( transactionId ) ) );

  /// Transaction on closed account
  auto tryMakeTransaction = [&accountBook, kTransactionAmount](
                              dbsc::UuidString const& accountId,
                              std::optional< std::reference_wrapper< dbsc::UuidString const > > otherPartyId ) {
    accountBook.deactivate( accountId );
    try {
      accountBook.makeTransaction( kTransactionAmount, "Internal account transactions", accountId, otherPartyId );
      BSLS_ASSERT( false );
    } catch ( dbsc::InactiveAccountException const& ) {
    }
    accountBook.activate( accountId );
  };
  tryMakeTransaction( secondAccountId, std::nullopt );
  tryMakeTransaction( accountId, std::nullopt );
  tryMakeTransaction( accountId, secondAccountId );
}

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
