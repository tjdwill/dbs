// dbsutl_helpers.h
//@PURPOSE: Provide helper functions that facilitate code implementation.
//
//@DESCRIPTION: This component serves as a housing for useful functions.
//  At the time of writing, there is no defined structure due to a lack of quantity,
//  but note that functions may be to be moved around over time.
#include <concepts>

namespace dbsutl {

template< typename EnumType, std::integral ReturnType = int >
constexpr auto enumAsIntegral( EnumType type ) -> ReturnType
{
  return static_cast< ReturnType >( type );
}
} // namespace dbsutl
