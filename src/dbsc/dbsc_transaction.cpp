// dbsc_transaction.cpp
#include "dbsc_transaction.h"

#include "dbsc_uuidstring.h"

namespace dbsc {

Transaction::Transaction( UuidString const& transactionId,
                          UuidString const& owningPartyId,
                          UuidString const& otherPartyID,
                          BloombergLP::bdldfp::Decimal64 amount,
                          TimeStamp timeStamp,
                          std::string const& notes )
  : mTransactionId( transactionId )
  , mOwningPartyId( owningPartyId )
  , mOtherPartyId( otherPartyID )
  , mAmount( amount )
  , mTimeStamp( timeStamp )
  , mNotes( notes )
{
}

auto Transaction::amount() const -> BloombergLP::bdldfp::Decimal64
{
  return mAmount;
}

auto Transaction::notes() const -> std::string const&
{
  return mNotes;
}

auto Transaction::owningPartyId() const -> UuidString const&
{
  return mOwningPartyId;
}

auto Transaction::otherPartyId() const -> UuidString const&
{
  return mOtherPartyId;
}

auto Transaction::transactionId() const -> UuidString const&
{
  return mTransactionId;
}

auto Transaction::timeStamp() const -> TimeStamp
{
  return mTimeStamp;
}

auto Transaction::isPair( Transaction const& a, Transaction const& b ) -> bool
{
  return ( a.amount() == -b.amount() && a.notes() == b.notes() && a.otherPartyId() == b.owningPartyId()
           && a.owningPartyId() == b.otherPartyId() && a.transactionId() == b.transactionId()
           && a.timeStamp() == b.timeStamp() );
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
