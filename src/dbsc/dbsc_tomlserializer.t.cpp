// dbsc_dbstomlserializer.t.cpp
#include <dbsc_account.h>
#include <dbsc_accountbook.h>
#include <dbsc_dbsserializer.h>
#include <dbsc_tomlserializer.h>
#include <dbsc_transaction.h>

#include <bdldfp_decimal.h>
#include <bsls_assert.h>

#include <filesystem>
#include <ranges>
#include <string_view>

namespace {
using namespace BloombergLP::bdldfp::DecimalLiterals;
constexpr int kTransactionCount = 10;

auto transactionAmount() -> BloombergLP::bdldfp::Decimal64
{
  static auto sAmount { "1.00"_d64 };
  return sAmount++;
}

auto createAccountBook() -> dbsc::AccountBook
{
  dbsc::AccountBook book { "tjdwill" };
  auto accountId1 = book.createAccount( "TestAccount1", "The first test account in the book" );
  auto accountId2 = book.createAccount( "TestAccount2", "The second test account in the book" );

  book.makeTransaction( "5000.00"_d64, "Initial Deposit", accountId1 );
  book.makeTransaction( "2000.00"_d64, "Initial Deposit", accountId2 );

  for ( auto const count : std::views::iota( 1, kTransactionCount + 1 ) ) {
    book.makeTransaction( transactionAmount(), std::format( "Transaction #{}", count ), accountId1, accountId2 );
  }

  return book;
}

auto accountBook() -> dbsc::AccountBook const&
{
  static dbsc::AccountBook const kAccountBook = createAccountBook();

  return kAccountBook;
}
} // namespace

int main()
{
  using namespace std::string_view_literals;
  std::filesystem::path const saveFile { "testAccountBook.toml"sv };
  dbsc::saveAccountBook< dbsc::TomlSerializer >( accountBook(), saveFile );

  auto const parsedAccountBook = dbsc::loadAccountBook< dbsc::TomlSerializer >( saveFile );
  BSLS_ASSERT( parsedAccountBook.owner() == accountBook().owner() );
  for ( auto const& [parsed, groundTruth] : std::views::zip( parsedAccountBook, accountBook() ) ) {
    BSLS_ASSERT( parsed == groundTruth );
  }

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
