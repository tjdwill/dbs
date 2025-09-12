// dbsc_registerexception.h
#ifndef INCLUDED_DBSC_REGISTEREXCEPTION
#define INCLUDED_DBSC_REGISTEREXCEPTION
//@PURPOSE: Provide a macro for creating new exceptions.
//
//@CLASSES:
//
//@MACROS:
//  DBSC_REGISTER_EXCEPTION: Contains common boilerplate code for defining a basic
//      subclass of std::exception.
//
//@DESCRIPTION: Since many exceptions share common implementation code, define a macro that
//  takes care of the boilerplate code. The macro is prefixed with the package name to
//  prevent name collisions.
//
/// Usage
/// -----
/// Example 1: Instantiation and Output
///
/// ```cpp
/// DBSC_REGISTER_EXCEPTION(TestException, "This is the default message.");
///
/// int main() {
///     try {
///         throw TestException()'
///     } catch (TestException const& error) {
///         std::cout << error.what() << "\n";
///     }
///     return 0;
/// }
/// ---
/// > ./testException
/// > This is the default message.
/// >
/// ```

/// Include necesary headers for exceptions. This is so other components using this macro
/// have the necessary components "for free".
#include <exception> // NOLINT
#include <string>    // NOLINT
/// @param EXCEPTION_NAME: the name of the exception class
/// @param DEFAULT_MESSAGE: the message that is passed to the default constructor.
///     @note This argument should always be enclosed in quotation marks, just like
///       defining a regular string literal. Even the empty default case should use `""`.
#define DBSC_REGISTER_EXCEPTION( EXCEPTION_NAME, DEFAULT_MESSAGE )                                                     \
  class EXCEPTION_NAME : public std::exception                                                                         \
  {                                                                                                                    \
  public:                                                                                                              \
    EXCEPTION_NAME( std::string const& message ) noexcept                                                              \
      : mMessage( message )                                                                                            \
    {                                                                                                                  \
    }                                                                                                                  \
    EXCEPTION_NAME() noexcept                                                                                          \
      : EXCEPTION_NAME( DEFAULT_MESSAGE )                                                                              \
    {                                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    [[nodiscard]] auto what() const noexcept -> char const*                                                            \
    {                                                                                                                  \
      return mMessage.c_str();                                                                                         \
    }                                                                                                                  \
                                                                                                                       \
  private:                                                                                                             \
    std::string mMessage;                                                                                              \
  };
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
