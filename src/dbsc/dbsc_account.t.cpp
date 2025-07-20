#include <dbsc/dbsc_account.h>
#include <dbsc/dbsc_transaction.h>
#include <dbsc/dbsc_uuidstring.h>

#include <bdldfp_decimal.h>

#include <cassert>     // for testing
#include <string>      // for testing
#include <string_view> // for testing
#include <type_traits> // for testing

using namespace BloombergLP;
using namespace bdldfp::DecimalLiterals;

namespace {
constexpr std::string_view kName { "Test Account" };
constexpr std::string_view kDescription {
  "This is an account intended to test dbsc::Account."
};

static auto sampleAccountMut() -> dbsc::Account&
{
  static dbsc::Account sTestAccount { std::string( kName ),
                                      std::string( kDescription ) };

  return sTestAccount;
}

static auto sampleAccount() -> dbsc::Account const&
{
  return sampleAccountMut();
}
} // namespace

static void testAccountAccessors()
{
  static_assert(
    std::is_same_v< decltype( sampleAccount() ), dbsc::Account const& > );
  assert( &sampleAccount() == &sampleAccountMut() );

  assert( not dbsc::UuidStringUtil::isNil( sampleAccount().id() ) );
  assert( sampleAccount().name() == std::string( kName ) );
  assert( sampleAccount().description() == std::string( kDescription ) );
  assert( sampleAccount().balance() == "0.0"_d64 );
}

int main()
{
  testAccountAccessors();
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
