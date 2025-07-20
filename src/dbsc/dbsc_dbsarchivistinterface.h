// dbsc_dbsarchivistinterface.h
#ifndef INCLUDED_DBSC_DBSARCHIVISTINTERFACE
#define INCLUDED_DBSC_DBSARCHIVISTINTERFACE

//@PURPOSE: Provide an abstract interface for DBS serialization.
//
//@CLASSES:
// dbsc::DbsArchivistInterface:  purely abstract base class that specifies the
// (de-)serialization interface.
//
//@DESCRIPTION: This component defines an abstract interface that specifies
// serialization and deserialization operations for DBS classes.

#include <iosfwd>

namespace dbsc {
class Transaction;
class Account;
class AccountBook;

/// This class is a protocol specifying the interface for serialization
/// operations concerning DBS classes. Implementers are responsible for
/// determining how the classes are serialized; the interface is only concerned
/// that the serialization and deserialization are done by the same entity.
class DbsArchivistInterface // NOLINT
{
public:
  virtual ~DbsArchivistInterface() = default;

  [[nodiscard]] virtual auto readTransaction( std::istream& source )
    -> Transaction                                                          = 0;
  [[nodiscard]] virtual auto readAccount( std::istream& source ) -> Account = 0;
  [[nodiscard]] virtual auto readAccountBook( std::istream& source )
    -> AccountBook                                                       = 0;
  virtual void writeTransaction( std::ostream& destination,
                                 Transaction const& )                    = 0;
  virtual void writeAccount( std::ostream& destination, Account const& ) = 0;
  virtual void writeAccountBook( std::ostream& destination,
                                 AccountBook const& )                    = 0;
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
