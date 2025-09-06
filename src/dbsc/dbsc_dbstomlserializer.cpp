// dbsc_dbstomlserializer.cpp
#include "dbsc_dbstomlserializer.h"

#include <dbsc/dbsc_account.h>
#include <dbsc/dbsc_accountbook.h>
#include <dbsc/dbsc_transaction.h>
#include <dbsc/dbsc_uuidstring.h>

#include <bdldfp_decimal.h>
#include <bdldfp_decimalutil.h>

#include <chrono>
#include <sstream>
#include <stdexcept>
#include <string_view>

namespace dbsc {

namespace {
  using namespace std::string_view_literals;
  constexpr auto kDateTimeParsingFormat = "%F %T"sv; // Date and Time
  std::filesystem::path const kTomlExtension { ".toml" };
  constexpr auto kAccountBookOwnerKey { "owner"sv };
  constexpr auto kAccountNameKey { "name"sv };
  constexpr auto kAccountDescriptionKey { "description"sv };
  constexpr auto kAccountTransactionsKey { "transactions"sv };
  constexpr auto kAccountOpenStatusKey { "isOpen"sv };
  constexpr auto kTransactionIdKey { "transactionId"sv };
  constexpr auto kTransactionOtherPartyIdKey { "otherPartyId"sv };
  constexpr auto kTransactionTimeStampKey { "timeStamp"sv };
  constexpr auto kTransactionAmountKey { "amount"sv };
  constexpr auto kTransactionNotesKey { "notes"sv };

  using TomlDateTimeType = std::string;
  using TomlCurrencyType = std::string;
} // namespace

/// @throws TODO: Create a macro for generating exception types. Use macro to create a
/// DbsSerialization error.
auto DbsTomlSerializer::readAccountBook( std::filesystem::path const& filePath ) -> AccountBook
{

  // Assume a valid
  // TOML file.
  if ( filePath.extension() != kTomlExtension ) {
    throw std::invalid_argument( std::format( "Invalid file extension. Expected {}. Got {}",
                                              filePath.filename().replace_extension( kTomlExtension ).c_str(),
                                              filePath.filename().c_str() ) );
  }
  InputType parsedFile  = toml::parse_file( filePath.c_str() );
  auto accountBookOwner = parsedFile.at( kAccountBookOwnerKey ).value< std::string >();
  {
    if ( !accountBookOwner.has_value() ) {
      throw std::runtime_error( std::format( "Incorrect type for key '{}'", kAccountBookOwnerKey ) );
    }
    /// Remove the account owner key so it can be ignored during iteration.
    parsedFile.erase( kAccountBookOwnerKey );
  }
  AccountBook accountBook { accountBookOwner.value() };

  for ( auto& [key, tomlValue] : parsedFile ) {
    if ( tomlValue.is_table() ) {
      auto accountId = UuidStringUtil::fromString<>( key.str() );

      auto valueTable = tomlValue.value< toml::table >().value();
      accountBook.addParsedAccount( readAccount( valueTable, accountId ) );
    }
  }

  return accountBook;
}

auto DbsTomlSerializer::readAccount( InputType& accountTomlTable, UuidString const& accountId ) -> Account
{
  auto const accountName        = accountTomlTable[kAccountNameKey].value< std::string >().value();
  auto const accountDescription = accountTomlTable[kAccountDescriptionKey].value< std::string >().value();
  Account account { accountId, accountName, accountDescription };
  account.openAccount(); // Ensure we can add the previous transactions.

  auto const transactionArrayOfTables = accountTomlTable[kAccountTransactionsKey].value< toml::array >().value();
  assert( transactionArrayOfTables.is_array_of_tables() );

  for ( auto const& transactionTableNode : transactionArrayOfTables ) {
    assert( transactionTableNode.is_table() );
    auto transactionTable = transactionTableNode.value< toml::table >().value();
    account.logTransaction( DbsTomlSerializer::readTransaction( transactionTable, accountId ) );
  }

  auto const accountIsOpen = accountTomlTable[kAccountOpenStatusKey].value< bool >().value();
  if ( !accountIsOpen ) {
    account.closeAccount();
  }

  return account;
}

auto DbsTomlSerializer::readTransaction( InputType& transactionTable, UuidString const& owningPartyId ) -> Transaction
{
  auto const transactionId =
    UuidStringUtil::fromString<>( transactionTable[kTransactionIdKey].value< std::string_view >().value() );
  auto const otherPartyId =
    UuidStringUtil::fromString<>( transactionTable[kTransactionOtherPartyIdKey].value< std::string_view >().value() );

  auto const amountString = transactionTable[kTransactionAmountKey].value< TomlCurrencyType >().value();
  BloombergLP::bdldfp::Decimal64 transactionAmount;
  BloombergLP::bdldfp::DecimalUtil::parseDecimal64( &transactionAmount, amountString );

  std::istringstream iss { transactionTable[kTransactionTimeStampKey].value< TomlDateTimeType >().value() };
  std::chrono::time_point< std::chrono::system_clock > timeStamp;
  std::chrono::from_stream( iss, kDateTimeParsingFormat.data(), timeStamp );

  auto const transactionNotes = transactionTable[kTransactionNotesKey].value< std::string >().value();

  return Transaction( transactionId, owningPartyId, otherPartyId, transactionAmount, timeStamp, transactionNotes );
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
