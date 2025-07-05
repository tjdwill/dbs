// dbsc_transaction.t.cpp
// Test driver for the dbsc::Transaction class.
#include "dbsc_uuid_string.h"

#include <dbsc/dbsc_transaction.h>

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
  auto transactionAmount = "100.00"_d64;
  auto transactionId     = UuidStringUtil::generate();
  auto otherPartyId      = UuidStringUtil::generate();
  auto timeStamp         = std::chrono::system_clock::now();
  std::string notes { ""s };
  Transaction transaction {
    transactionId, otherPartyId, transactionAmount, timeStamp, notes
  };

  assert(transaction.transactionId() == transactionId);
  assert(transaction.otherPartyId() == otherPartyId);
  assert(transaction.amount() == transactionAmount);
  assert(transaction.timeStamp() == timeStamp);
  assert(transaction.notes() == notes);
}
} // namespace

int main()
{
  testGetters();
}
