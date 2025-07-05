// decimalQt.m.cpp
// Learning how to use BDL decimal floating point and Qt types.

#include <bdldfp_decimal.h>
#include <bdldfp_decimalutil.h>
#include <bsl_iostream.h>
#include <bslstl_ostringstream.h>
#include <bslstl_string.h>

#include <QString>
#include <cassert>

using namespace BloombergLP;
using namespace bdldfp::DecimalLiterals;

int main()
{

  bdldfp::Decimal64 fp { 27.0000_d64 };
  // convert the decimal value to a string
  bsl::ostringstream oss {};
  oss << fp;
  QString fpQt { oss.str().data() };
  bsl::cout << "Stringified: " << qPrintable(fpQt) << "\n";
  // Try to convert a string into a number
  QString const newFpQt { "27.0" };
  bdldfp::Decimal64 newfp;
  bdldfp::DecimalUtil::parseDecimal64(&newfp, qPrintable(newFpQt));

  assert(newfp == fp);
}
