// dbsc_tomlserializer.h
#ifndef INCLUDED_DBSC_TOMLSERIALIZER
#define INCLUDED_DBSC_TOMLSERIALIZER

//@PURPOSE: Provide a TOML-specific implementation of dbsc::DbsSerializer
//
//@CLASSES:
//  dbsc::TomlSerializer: Serializer for the DBS classes in a TOML interface.
//
//@DESCRIPTION: This component defines a specific implementation for
//  dbsc::DbsSerializer.

#include <dbsc_dbscserializer.h>

#include <toml++/toml.hpp>

#include <filesystem>

namespace dbsc {
class Transaction;
class Account;
class AccountBook;
class UuidString;

/// This class adheres to the dbsc::DbscSerializer concept.
class TomlSerializer
{
public:
  using InputType  = toml::table;
  using OutputType = InputType;

  [[nodiscard]] static auto readAccountBook( std::filesystem::path const& filePath ) -> AccountBook;
  [[nodiscard]] static auto readAccountInternal( InputType& inSource, UuidString const& accountId ) -> Account;
  [[nodiscard]] static auto readTransactionInternal( InputType& inSource, UuidString const& owningPartyId )
    -> Transaction;
  static void writeAccountBook( AccountBook const& accountBook, std::filesystem::path const& filePath );
  static void writeAccountInternal( OutputType& oDestinationBuf, Account const& account );
  static void writeTransactionInternal( OutputType& oDestinationBuf, Transaction const& transaction );
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
