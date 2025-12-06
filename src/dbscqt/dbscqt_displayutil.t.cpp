// dbscqt_displayutil.t.cpp
#include <dbscqt_displayutil.h>

#include <cassert>

namespace {
struct DisplayUtilTest
{
  struct AccountNameWithShortenedUuid
  {
    AccountNameWithShortenedUuid()
    {
      QString const name           = "External";
      QString const expectedString = QString( "%1 (%2)" ).arg( name ).arg( "00000000" );
      QString const shortenedName  = dbscqt::DisplayUtil::accountNameWithShortenedUuid( QUuid(), name );

      assert( shortenedName == expectedString );
    }
  };

  DisplayUtilTest() { AccountNameWithShortenedUuid(); }
};
} // namespace

int main()
{
  DisplayUtilTest();
}
