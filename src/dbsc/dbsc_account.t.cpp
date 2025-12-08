// dbsc_account.t.cpp
#include <dbsc_account.h>
#include <dbsc_transaction.h>
#include <dbsc_uuidstring.h>

#include <bdldfp_decimal.h>
#include <bsls_assert.h>

#include <chrono>      // for testing
#include <string>      // for testing
#include <string_view> // for testing
#include <type_traits> // for testing

using namespace BloombergLP;
using namespace bdldfp::DecimalLiterals;
using namespace std::string_literals;

namespace {
constexpr std::string_view kAccountName { "Test Account" };
constexpr std::string_view kAccountDescription { "This is an account intended to test dbsc::Account." };

static auto sampleAccountMut() -> dbsc::Account&
{
  static dbsc::Account sTestAccount { std::string( kAccountName ), std::string( kAccountDescription ) };

  return sTestAccount;
}

static auto sampleAccount() -> dbsc::Account const&
{
  return sampleAccountMut();
}

bdldfp::Decimal64 const kTransactionAmount { "1000.27"_d64 };
std::string const kTransactionDescription { "Test Transaction"s };

dbsc::Transaction const kExampleTransaction { dbsc::UuidStringUtil::generate(), sampleAccount().id(),
                                              dbsc::UuidStringUtil::generate(), kTransactionAmount,
                                              std::chrono::system_clock::now(), kTransactionDescription };

} // namespace

static void testAccountAccessors()
{
  static_assert( std::is_same_v< decltype( sampleAccount() ), dbsc::Account const& > );
  BSLS_ASSERT( &sampleAccount() == &sampleAccountMut() );

  BSLS_ASSERT( not dbsc::UuidStringUtil::isNil( sampleAccount().id() ) );
  BSLS_ASSERT( sampleAccount().name() == std::string( kAccountName ) );
  BSLS_ASSERT( sampleAccount().description() == std::string( kAccountDescription ) );
  BSLS_ASSERT( sampleAccount().balance() == "0.0"_d64 );
}

int main()
{
  testAccountAccessors();

  // Test transaction retrieval
  sampleAccountMut().logTransaction( kExampleTransaction );
  dbsc::UuidString transactionId;
  for ( auto const& [id, _] : sampleAccount() ) {
    BSLS_ASSERT( sampleAccount().transactionCount() == 1 );
    transactionId = id;
  }
  dbsc::Transaction const& transaction = sampleAccount().transaction( transactionId );
  BSLS_ASSERT( transaction.amount() == kTransactionAmount );
  BSLS_ASSERT( transaction.notes() == kTransactionDescription );

  // Attempt to add a duplicate transaction.
  try {
    sampleAccountMut().logTransaction( kExampleTransaction );
  } catch ( dbsc::DuplicateUuidException const& ) {
  }

  sampleAccountMut().closeAccount();
  BSLS_ASSERT( not sampleAccount().isOpen() );
  BSLS_ASSERT( sampleAccount().contains( transactionId ) );
  BSLS_ASSERT( sampleAccount() == sampleAccount() );
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
