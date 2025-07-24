// dbsc_transaction.t.cpp
// Test driver for the dbsc::Transaction class.
#include <dbsc/dbsc_transaction.h>
#include <dbsc/dbsc_uuidstring.h>

#include <bdldfp_decimal.h>

#include <cassert>
#include <chrono>
#include <string>

namespace {
using namespace dbsc;
using namespace BloombergLP;
using namespace bdldfp::DecimalLiterals;
using namespace std::string_literals;

static void testGetters()
{
  auto const transactionAmount = "100.00"_d64;
  auto const transactionId     = UuidStringUtil::generate();
  auto const owningPartyId     = UuidStringUtil::generate();
  auto const otherPartyId      = UuidStringUtil::generate();
  auto const timeStamp         = std::chrono::system_clock::now();
  std::string const notes { ""s };

  Transaction const transaction { transactionId, owningPartyId,
                                  otherPartyId,  transactionAmount,
                                  timeStamp,     notes };

  assert( transaction.transactionId() == transactionId );
  assert( transaction.owningPartyId() == owningPartyId );
  assert( transaction.otherPartyId() == otherPartyId );
  assert( transaction.amount() == transactionAmount );
  assert( transaction.timeStamp() == timeStamp );
  assert( transaction.notes() == notes );
}
} // namespace

int main()
{
  testGetters();
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
