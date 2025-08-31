// dbsc_dbsserializer.h
#ifndef INCLUDED_DBSC_DBSSERIALIZER
#define INCLUDED_DBSC_DBSSERIALIZER

//@PURPOSE: Provide an abstract interface for DBS serialization.
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

#include <filesystem>

namespace dbsc {
class Transaction;
class Account;
class AccountBook;

/// These concepts is a protocol that specifies the interface for serialization
/// operations concerning DBS classes. Implementers are responsible for
/// determining how the classes are serialized; the interface is only concerned
/// that the serialization and deserialization are done by the same entity.

/// `Serializer::InputType` refers to the source from which the dbsc classes are
/// parsed.
template< typename Serializer >
concept DbsReader = requires( typename Serializer::InputType& io, std::filesystem::path const& filePath ) {
  Serializer::readTransaction( io )->Transaction;
  Serializer::readAccount( io )->Account;
  /// Parse the accountBook from file.
  Serializer::readAccountBook( filePath )->AccountBook;
};
/// `Serializer::OutputType` refers to the source to which the dbsc classes are
/// written.
template< typename Serializer >
concept DbsWriter = requires( typename Serializer::OutputType& io,
                              Transaction const& transaction,
                              Account const& account,
                              AccountBook const& accountBook,
                              std::filesystem::path const& filePath ) {
  Serializer::writeTransaction( io, transaction );
  Serializer::writeAccount( io, account );
  /// Write the entire account book to file.
  Serializer::writeAccountBook( accountBook, filePath );
};

template< typename Serializer >
concept DbsSerializer = DbsReader< Serializer > && DbsWriter< Serializer >;

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
