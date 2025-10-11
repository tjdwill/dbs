// dbsc_transaction.cpp
#include "dbsc_transaction.h"

#include <bdldfp_decimalutil.h>
#include <bslstl_ostringstream.h>

#include <format>
#include <sstream>
#include <string_view>

namespace dbsc {

namespace {
  using namespace std::string_view_literals;
  constexpr std::string_view kTimeStampConversionFormat { "%F %T%Z"sv };
} // namespace

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

auto TransactionUtil::currencyAsString( BloombergLP::bdldfp::Decimal64 const& amount ) -> std::string
{
  bsl::ostringstream oss {};
  oss << amount;
  return std::string( oss.str().data() );
}

auto TransactionUtil::currencyFromString( std::string_view str ) -> BloombergLP::bdldfp::Decimal64
{
  BloombergLP::bdldfp::Decimal64 amount;
  BloombergLP::bdldfp::DecimalUtil::parseDecimal64( &amount, str.data() );
  return amount;
}

auto constexpr TransactionUtil::timestampConversionFormat() -> std::string_view
{
  return kTimeStampConversionFormat;
}

auto TransactionUtil::timestampAsString( TimeStamp const& timestamp ) -> std::string
{
  std::ostringstream oss {};
  oss << std::format( "{0:%F} {0:%T%Z}", timestamp );
  return oss.str();
}

auto TransactionUtil::timestampFromString( std::string_view borrowedStr ) -> TimeStamp
{
  std::istringstream iss { borrowedStr.data() };
  TimeStamp parsedTimeStamp;
  std::chrono::from_stream( iss, kTimeStampConversionFormat.data(), parsedTimeStamp );

  if ( not( iss || iss.eof() ) ) {
    throw std::invalid_argument( std::format(
      "Input string '{0}' did not adhere to the time format '{1}'", borrowedStr, kTimeStampConversionFormat ) );
  }
  return parsedTimeStamp;
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
