// dbsc_transaction.t.cpp
// Test driver for the dbsc::Transaction class.
#include "bde/include/bdldfp_decimal.fwd.h"

#include <dbsc_transaction.h>
#include <dbsc_uuidstring.h>

#include <bdldfp_decimal.h>
#include <bsls_assert.h> // for testing

#include <chrono>
#include <string>

namespace {
using namespace dbsc;
using namespace BloombergLP;
using namespace bdldfp::DecimalLiterals;
using namespace std::string_literals;

static void testDecimalConversion()
{
  BloombergLP::bdldfp::Decimal64 testAmount  = "12398570.234"_d64;
  BloombergLP::bdldfp::Decimal64 testAmount2 = "-12398570.234"_d64;
  auto roundTripAmount  = TransactionUtil::currencyFromString( TransactionUtil::currencyAsString( testAmount ) );
  auto roundTripAmount2 = TransactionUtil::currencyFromString( TransactionUtil::currencyAsString( testAmount2 ) );

  BSLS_ASSERT( testAmount == roundTripAmount );
  BSLS_ASSERT( testAmount2 == roundTripAmount2 );
  BSLS_ASSERT( roundTripAmount != testAmount2 );
  BSLS_ASSERT( roundTripAmount2 != testAmount );
}

static void testTimeStampConversion()
{
  TimeStamp const time = std::chrono::system_clock::now();
  auto parsedTime      = TransactionUtil::timestampFromString( TransactionUtil::timestampAsString( time ) );

  BSLS_ASSERT( parsedTime == time );
}

static void testGetters()
{
  auto const transactionAmount = "100.00"_d64;
  auto const transactionId     = UuidStringUtil::generate();
  auto const owningPartyId     = UuidStringUtil::generate();
  auto const otherPartyId      = UuidStringUtil::generate();
  auto const timeStamp         = std::chrono::system_clock::now();
  std::string const notes { ""s };

  Transaction const transaction { transactionId, owningPartyId, otherPartyId, transactionAmount, timeStamp, notes };

  BSLS_ASSERT( transaction.transactionId() == transactionId );
  BSLS_ASSERT( transaction.owningPartyId() == owningPartyId );
  BSLS_ASSERT( transaction.otherPartyId() == otherPartyId );
  BSLS_ASSERT( transaction.amount() == transactionAmount );
  BSLS_ASSERT( transaction.timeStamp() == timeStamp );
  BSLS_ASSERT( transaction.notes() == notes );
}

static void testEquality()
{
  UuidString const transactionId     = dbsc::UuidStringUtil::generate();
  UuidString const accountIdA        = dbsc::UuidStringUtil::generate();
  UuidString const accountIdB        = dbsc::UuidStringUtil::generate();
  auto const timeStamp               = std::chrono::system_clock::now();
  auto const transactionAmount       = "2000"_d64;
  std::string const transactionNotes = "";
  Transaction const t1 { transactionId, accountIdA, accountIdB, transactionAmount, timeStamp, transactionNotes };
  Transaction const t2 { transactionId, accountIdB, accountIdA, -transactionAmount, timeStamp, transactionNotes };

  BSLS_ASSERT( t1 != t2 );
  BSLS_ASSERT( t2 != t1 );
  BSLS_ASSERT( t1 == t1 );
  BSLS_ASSERT( t2 == t2 );
  BSLS_ASSERT( Transaction::isPair( t1, t2 ) );
}
} // namespace

int main()
{
  testGetters();
  testEquality();
  testTimeStampConversion();
  testDecimalConversion();

  return 0;
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
