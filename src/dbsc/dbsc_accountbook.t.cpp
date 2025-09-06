// dbsc_accountbook.t.cpp
#include <dbsc/dbsc_account.h>
#include <dbsc/dbsc_accountbook.h>
#include <dbsc/dbsc_uuidstring.h>

#include <bdldfp_decimal.h>

#include <cassert> // for testing
#include <string>
#include <string_view> // for testing

namespace {

using namespace BloombergLP::bdldfp::DecimalLiterals;
using namespace std::string_view_literals;
std::string_view constexpr kOwnerName { "tjdwill"sv };

} // namespace

int main()
{

  dbsc::AccountBook accountBook { std::string { kOwnerName } };

  // Test: adding accounts
  assert( accountBook.accountCount() == 0 );
  auto const accountId = accountBook.createAccount( "TestAccount", "Some Description" );
  assert( accountBook.accountCount() == 1 );
  accountBook.addParsedAccount( dbsc::Account( "SomeOwner", "" ) );
  assert( accountBook.accountCount() == 2 );

  // Test accessors
  try {
    auto const badAccountAccess = accountBook.account( {} );
  } catch ( dbsc::NonExistentAccountException const& ) {
  }

  assert( accountBook.owner() == kOwnerName );
  assert( accountBook.account( accountId ) == accountBook.account( accountId ) );
  assert( accountBook.cbegin() != accountBook.cend() );
  assert( accountBook.account( accountId ).isOpen() );

  // Account ops
  accountBook.closeAccount( accountId );
  assert( not accountBook.account( accountId ).isOpen() );
  accountBook.openAccount( accountId );
  assert( accountBook.account( accountId ).isOpen() );

  /// External withdrawal
  auto const kTransactionAmount = "100.00"_d64;
  accountBook.makeTransaction( kTransactionAmount, "transactionNotes", accountId );
  assert( accountBook.account( accountId ).balance() == kTransactionAmount );

  auto const secondAccountId = accountBook.createAccount( "SecondAccount", "Second Description" );

  auto const transactionId =
    accountBook.makeTransaction( kTransactionAmount, "Internal account transactions", secondAccountId, accountId );
  assert( accountBook.account( accountId ).balance() == BloombergLP::bdldfp::Decimal64 {} );
  assert( accountBook.account( secondAccountId ).balance() == kTransactionAmount );
  assert( dbsc::Transaction::isPair( accountBook.account( secondAccountId ).transaction( transactionId ),
                                     accountBook.account( accountId ).transaction( transactionId ) ) );
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
