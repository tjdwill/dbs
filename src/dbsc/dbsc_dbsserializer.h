// dbsc_dbsserializer.h
#ifndef INCLUDED_DBSC_DBSSERIALIZER
#define INCLUDED_DBSC_DBSSERIALIZER

//@PURPOSE: Provide an abstract interface for DBS serialization.
//
//@CLASSES:
//  dbsc::DbsSerializationException: Thrown when an error occurs in (de)serialization..
//
//@CONCEPTS:
// dbsc::DbsReader:  A concept that illustrates the
//   de-serialization interface.
// dbsc::DbsWriter:  A concept that illustrates the
//   serialization interface.
// dbsc::DbsSerializer:  A concepts that combines the encoding and decoding
//   interfaces.
//
//@DESCRIPTION: This component defines an abstract interface that specifies
// serialization and deserialization operations for DBS classes.
#include <dbsc_accountbook.h>
#include <dbsc_registerexception.h>

#include <concepts>
#include <filesystem>

namespace dbsc {
class Transaction;
class Account;
class AccountBook;
class UuidString;

DBSC_REGISTER_EXCEPTION( DbsSerializationException, "An error occurred during (de)serialization." );

/// These concepts is a protocol that specifies the interface for serialization
/// operations concerning DBS classes. Implementers are responsible for
/// determining how the classes are serialized; the interface is only concerned
/// that the serialization and deserialization are done by the same entity.
/// @note Any of the read/write functions can @throw @c DbsSerializationException.

/// `Serializer::InputType` refers to the source from which the dbsc classes are
/// parsed.
template< typename Serializer >
concept DbsReader =
  requires( typename Serializer::InputType& io, std::filesystem::path const& filePath, UuidString const& idString ) {
    /// Parse the accountBook from file.
    { Serializer::readAccountBook( filePath ) } -> std::same_as< AccountBook >;
    // These functions are internal since each implementing type could have different
    // InputTypes.
    { Serializer::readAccountInternal( io, idString ) } -> std::same_as< Account >;
    { Serializer::readTransactionInternal( io, idString ) } -> std::same_as< Transaction >;
  };
/// `Serializer::OutputType` refers to the source to which the dbsc classes are
/// written.
template< typename Serializer >
concept DbsWriter = requires( typename Serializer::OutputType& io,
                              Transaction const& transaction,
                              Account const& account,
                              AccountBook const& accountBook,
                              std::filesystem::path const& filePath ) {
  /// Write the entire account book to file.
  Serializer::writeAccountBook( accountBook, filePath );
  // These functions are internal since each implementing type could have different
  // OutputTypes.
  Serializer::writeAccountInternal( io, account );
  Serializer::writeTransactionInternal( io, transaction );
};

template< typename Serializer >
concept DbsSerializer = DbsReader< Serializer > && DbsWriter< Serializer >;

template< typename Serializer >
  requires dbsc::DbsSerializer< Serializer >
auto saveAccountBook( dbsc::AccountBook const& accountRecord, std::filesystem::path const& filePath )
{
  Serializer::writeAccountBook( accountRecord, filePath );
}

template< typename Serializer >
  requires dbsc::DbsSerializer< Serializer >
auto loadAccountBook( std::filesystem::path const& filePath ) -> dbsc::AccountBook
{
  return Serializer::readAccountBook( filePath );
}

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
