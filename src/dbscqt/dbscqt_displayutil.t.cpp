// dbscqt_displayutil.t.cpp
#include <dbsc_uuidstring.h>
#include <dbscqt_displayutil.h>

#include <bdldfp_decimal.h>
#include <bsls_assert.h>

#include <QString>

#include <concepts>

namespace {

template< typename T, typename U, typename Converter, typename Inverter >
  requires requires( T const& type, U const& invertedType, Converter convert, Inverter invert ) {
    { convert( type ) } -> std::same_as< U >;
    { invert( invertedType ) } -> std::same_as< T >;
  }
auto testRoundTrip( T const& startingValue, Converter convert, Inverter invert ) -> bool
{
  return startingValue == invert( convert( invert( convert( startingValue ) ) ) );
}

struct DisplayUtilTest
{
  struct AccountNameWithShortenedUuid
  {
    AccountNameWithShortenedUuid()
    {
      QString const name           = "External";
      QString const expectedString = QString( "%1 (%2)" ).arg( name ).arg( "00000000" );
      QString const shortenedName  = dbscqt::DisplayUtil::accountNameWithShortenedUuid( QUuid(), name );

      BSLS_ASSERT( shortenedName == expectedString );
    }
  };

  struct DecimalFpRoundTrip
  {
    DecimalFpRoundTrip()
    {
      using namespace BloombergLP::bdldfp::DecimalLiterals;
      using BloombergLP::bdldfp::Decimal64;
      Decimal64 const sampleAmount { "12395.52"_d64 };
      Decimal64 const negatedAmount { -sampleAmount };
      bool const trip1Passed = testRoundTrip< Decimal64,
                                              QString,
                                              decltype( dbscqt::DisplayUtil::toDecimalQString ),
                                              decltype( dbscqt::DisplayUtil::toDecimal64 ) >(
        sampleAmount, dbscqt::DisplayUtil::toDecimalQString, dbscqt::DisplayUtil::toDecimal64 );
      BSLS_ASSERT( trip1Passed );

      bool const trip2Passed = testRoundTrip< Decimal64,
                                              QString,
                                              decltype( dbscqt::DisplayUtil::toDecimalQString ),
                                              decltype( dbscqt::DisplayUtil::toDecimal64 ) >(
        negatedAmount, dbscqt::DisplayUtil::toDecimalQString, dbscqt::DisplayUtil::toDecimal64 );

      BSLS_ASSERT( trip2Passed );
    }
  };

  struct UuidConversionRoundTrip
  {
    UuidConversionRoundTrip()
    {
      auto const uuidString = dbsc::UuidStringUtil::generate();
      BSLS_ASSERT( ( testRoundTrip< dbsc::UuidString, QUuid >(
        uuidString, dbscqt::DisplayUtil::toQUuid, dbscqt::DisplayUtil::toDbscUuidString ) ) );

      auto const quuid = QUuid::createUuid();
      BSLS_ASSERT( ( testRoundTrip< QUuid, dbsc::UuidString >(
        quuid, dbscqt::DisplayUtil::toDbscUuidString, dbscqt::DisplayUtil::toQUuid ) ) );
    }
  };

  DisplayUtilTest()
  {
    AccountNameWithShortenedUuid();
    DecimalFpRoundTrip();
    UuidConversionRoundTrip();
  }
};
} // namespace

int main()
{
  DisplayUtilTest();
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
