// dbsc_transaction.h
#ifndef INCLUDED_DBSC_TRANSACTION
#define INCLUDED_DBSC_TRANSACTION

//@PURPOSE: Representation for DBS transactions.
//
//@CLASSES:
//  dbsc::Transaction: in-memory representation of an account transaction.
//
//@DESCRIPTION: This component defines the representation of a transaction in
// DBS. Required data are:
//  - Unique Transaction ID
//  - Other Party's ID
//  - Amount ($)
//  - DateTime
//  - Extra Notes

#include <dbsc/dbsc_uuidstring.h>

#include <bdldfp_decimal.h>

#include <chrono>
#include <string>

namespace dbsc {
using TimeStamp = std::chrono::time_point< std::chrono::system_clock >;

/// Represents a transaction between two entities. A negative amount implies the
/// account to which this object belongs had a withdrawl. A positive amount
/// indicates a deposit. If the otherPartyID is nil (see UuidString), the
/// otherParty is some unspecified external source (ex. payment from a source of
/// income.). Generally, dbsc::Transactions are intended to be made in pairs.
/// The two pairs will have swapped party IDs and transaction amounts of
/// opposing signs.
class Transaction
{
public:
  [[nodiscard]] explicit Transaction( UuidString const& transactionId,
                                      UuidString const& owningPartyId,
                                      UuidString const& otherPartyId,
                                      BloombergLP::bdldfp::Decimal64 amount,
                                      TimeStamp timeStamp,
                                      std::string const& notes );

  [[nodiscard]] auto amount() const -> BloombergLP::bdldfp::Decimal64;
  [[nodiscard]] auto notes() const -> std::string const&;
  [[nodiscard]] auto owningPartyId() const -> UuidString const&;
  [[nodiscard]] auto otherPartyId() const -> UuidString const&;
  [[nodiscard]] auto timeStamp() const -> TimeStamp;
  [[nodiscard]] auto transactionId() const -> UuidString const&;

private:
  UuidString mTransactionId;
  /// The account that owns this transaction object.
  UuidString mOwningPartyId;
  /// The other entity involved in the transaction.
  UuidString mOtherPartyId;
  BloombergLP::bdldfp::Decimal64 mAmount;
  TimeStamp mTimeStamp;
  std::string mNotes {};
};
} // namespace dbsc
#endif // include guard

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
