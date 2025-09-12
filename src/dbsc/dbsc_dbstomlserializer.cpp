// dbsc_dbstomlserializer.cpp
#include "dbsc_dbstomlserializer.h"

#include <dbsc_account.h>
#include <dbsc_accountbook.h>
#include <dbsc_transaction.h>
#include <dbsc_uuidstring.h>

#include <bdldfp_decimal.h>
#include <bdldfp_decimalutil.h>
#include <bslstl_iosfwd.h>
#include <bslstl_ostringstream.h>
#include <bslstl_string.h>

#include <cassert>
#include <chrono>
#include <format>
#include <fstream>
#include <sstream>
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

auto DbsTomlSerializer::readAccountBook( std::filesystem::path const& filePath ) -> AccountBook
{

  // Assume a valid TOML file.
  if ( filePath.extension() != kTomlExtension ) {
    throw DbsSerializationException( std::format( "Invalid file extension. Expected {}. Got {}",
                                                  filePath.filename().replace_extension( kTomlExtension ).c_str(),
                                                  filePath.filename().c_str() ) );
  }
  InputType parsedFile  = toml::parse_file( filePath.c_str() );
  auto accountBookOwner = parsedFile.at( kAccountBookOwnerKey ).value< std::string >();
  {
    if ( !accountBookOwner.has_value() ) {
      throw DbsSerializationException( std::format( "Incorrect type for key '{}'", kAccountBookOwnerKey ) );
    }
    /// Remove the account owner key so it can be ignored during iteration.
    parsedFile.erase( kAccountBookOwnerKey );
  }
  AccountBook accountBook { accountBookOwner.value() };

  for ( auto& [key, tomlValue] : parsedFile ) {
    if ( tomlValue.is_table() ) {
      auto accountId = UuidStringUtil::fromString<>( key.str() );

      auto* valueTable = tomlValue.as_table();
      assert( valueTable );
      accountBook.addParsedAccount( readAccountInternal( *valueTable, accountId ) );
    }
  }

  return accountBook;
}

auto DbsTomlSerializer::readAccountInternal( InputType& accountTomlTable, UuidString const& accountId ) -> Account
{
  auto const accountName        = accountTomlTable[kAccountNameKey].value< std::string >().value();
  auto const accountDescription = accountTomlTable[kAccountDescriptionKey].value< std::string >().value();
  Account account { accountId, accountName, accountDescription };
  account.openAccount(); // Ensure we can add the previous transactions.

  auto* const transactionArrayOfTables = accountTomlTable[kAccountTransactionsKey].as_array();
  assert( transactionArrayOfTables->is_array_of_tables() );
  for ( auto& transactionTableNode : *transactionArrayOfTables ) {
    assert( transactionTableNode.is_table() );
    auto* transactionTable = transactionTableNode.as_table();
    account.logTransaction( DbsTomlSerializer::readTransactionInternal( *transactionTable, accountId ) );
  }

  auto const accountIsOpen = accountTomlTable[kAccountOpenStatusKey].value< bool >().value();
  if ( !accountIsOpen ) {
    account.closeAccount();
  }

  return account;
}

auto DbsTomlSerializer::readTransactionInternal( InputType& transactionTable, UuidString const& owningPartyId )
  -> Transaction
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

void DbsTomlSerializer::writeAccountBook( AccountBook const& accountBook, std::filesystem::path const& filePath )
{
  toml::table topLevelTable;
  toml::value< std::string > accountOwner { accountBook.owner() };
  topLevelTable.insert( kAccountBookOwnerKey, accountOwner );

  for ( auto const& [accountId, account] : accountBook ) {
    toml::table accountTable;
    DbsTomlSerializer::writeAccountInternal( accountTable, account );
    topLevelTable.insert( accountId.view(), std::move( accountTable ) );
  }

  std::ofstream ofs { filePath };
  if ( !ofs ) {
    throw DbsSerializationException( std::format( "Could not write to file '{}'.", filePath.c_str() ) );
  }
  ofs << topLevelTable << "\n";
}

void DbsTomlSerializer::writeAccountInternal( OutputType& accountTable, Account const& account )
{
  toml::value< std::string > const accountName { account.name() };
  toml::value< std::string > const accountDescription { account.description() };
  toml::value< bool > accountIsOpen { account.isOpen() };
  accountTable.insert( kAccountNameKey, accountName );
  accountTable.insert( kAccountDescriptionKey, accountDescription );
  accountTable.insert( kAccountOpenStatusKey, accountIsOpen );

  toml::array transactionArray {};
  for ( auto const& [_, transaction] : account ) {
    toml::table transactionTable;
    DbsTomlSerializer::writeTransactionInternal( transactionTable, transaction );
    transactionArray.push_back( std::move( transactionTable ) );
  }
  assert( transactionArray.is_array_of_tables() );
  accountTable.insert( kAccountTransactionsKey, std::move( transactionArray ) );
}

void DbsTomlSerializer::writeTransactionInternal( OutputType& transactionTable, Transaction const& transaction )
{
  toml::value< std::string > const transactionId { transaction.transactionId().view() };
  toml::value< std::string > const otherPartyId { transaction.otherPartyId().view() };
  bsl::ostringstream oss;
  oss << transaction.amount();
  toml::value< TomlCurrencyType > const transactionAmount { oss.str() };
  toml::value< TomlDateTimeType > const dateTime { std::format( "{0:%F} {0:%T}", transaction.timeStamp() ) };
  toml::value< std::string > const notes { transaction.notes() };

  transactionTable.insert( kTransactionIdKey, std::move( transactionId ) );
  transactionTable.insert( kTransactionOtherPartyIdKey, std::move( otherPartyId ) );
  transactionTable.insert( kTransactionAmountKey, std::move( transactionAmount ) );
  transactionTable.insert( kTransactionTimeStampKey, std::move( dateTime ) );
  transactionTable.insert( kTransactionNotesKey, std::move( notes ) );
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
