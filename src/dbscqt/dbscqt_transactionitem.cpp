// dbscqt_transactionitem.cpp
#include "dbscqt_transactionitem.h"

#include <dbscqt_displayutil.h>

namespace dbscqt {
class TransactionItem::Private
{
public:
  explicit Private( TransactionItemData const& transactionData )
    : mData( transactionData )
  {
  }

  TransactionItemData mData;
};
} // namespace dbscqt

dbscqt::TransactionItem::TransactionItem( dbscqt::TransactionItemData const& transactionData )
  : mImp( std::make_unique< Private >( transactionData ) )
{
}

dbscqt::TransactionItem::~TransactionItem() = default;

auto dbscqt::TransactionItem::amount() const -> QString const&
{
  return mImp->mData.mTransactionAmount;
}

auto dbscqt::TransactionItem::notes() const -> QString const&
{
  return mImp->mData.mNotes;
}

auto dbscqt::TransactionItem::otherPartyId() const -> QUuid
{
  return mImp->mData.mOtherPartyId;
}

auto dbscqt::TransactionItem::timeStamp() const -> QDateTime const&
{
  return mImp->mData.mTimeStamp;
}

auto dbscqt::TransactionItem::transactionId() const -> QUuid
{
  return mImp->mData.mTransactionId;
}

auto dbscqt::TransactionItem::otherPartyDisplayName() const -> QString
{
  return mImp->mData.mOtherPartyId == QUuid() ? "External"
                                              : dbscqt::DisplayUtil::accountNameWithShortenedUuid(
                                                  mImp->mData.mOtherPartyId, mImp->mData.mOtherPartyAccountName );
}

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
