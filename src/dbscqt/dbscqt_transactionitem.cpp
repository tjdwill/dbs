// dbscqt_transactionitem.cpp
#include "dbscqt_transactionitem.h"

#include <dbsc_account.h>
#include <dbsc_accountbook.h>
#include <dbscqt_displayutil.h>

#include <algorithm>
#include <ranges>

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
  return mImp->mData.mOtherPartyId == dbscqt::DisplayUtil::externalPartyId()
         ? dbscqt::DisplayUtil::externalPartyLabel()
         : dbscqt::DisplayUtil::accountNameWithShortenedUuid( mImp->mData.mOtherPartyId,
                                                              mImp->mData.mOtherPartyAccountName );
}

auto dbscqt::createTransactionItemData( dbsc::Transaction const& transaction, dbsc::AccountBook const& accountBook )
  -> dbscqt::TransactionItemData
{

  QDateTime const timestamp       = dbscqt::DisplayUtil::toQDateTime( transaction.timestamp() );
  QString const transactionAmount = dbscqt::DisplayUtil::toDecimalQString( transaction.amount() );
  QUuid const otherPartyId        = dbscqt::DisplayUtil::toQUuid( transaction.otherPartyId() );
  QString const otherPartyAccountName =
    otherPartyId.isNull() ? dbscqt::DisplayUtil::externalPartyLabel()
                          : QString::fromStdString( accountBook.account( transaction.otherPartyId() ).name() );

  return {
    .mTimeStamp             = timestamp,
    .mTransactionAmount     = transactionAmount,
    .mNotes                 = QString::fromStdString( transaction.notes() ),
    .mOtherPartyAccountName = otherPartyAccountName,
    .mTransactionId         = dbscqt::DisplayUtil::toQUuid( transaction.transactionId() ),
    .mOtherPartyId          = otherPartyId,
  };
};

auto dbscqt::createTransactionItems( dbsc::Account const& account, dbsc::AccountBook const& accountBook )
  -> std::vector< std::unique_ptr< dbscqt::TransactionItem > >
{
  auto transactionsSortedByDescendingDate =
    account | std::views::transform( []( auto const& transaction ) { return std::cref( transaction ); } )
    | std::ranges::to< std::vector >();
  std::ranges::sort( transactionsSortedByDescendingDate, std::greater<>(), []( auto&& item ) -> dbsc::TimeStamp {
    auto const& [_, transaction] = item.get();
    return transaction.timestamp();
  } );

  std::vector< std::unique_ptr< dbscqt::TransactionItem > > items;
  items.reserve( account.transactionCount() );
  for ( auto const& item : transactionsSortedByDescendingDate ) {
    auto const& [id, transaction] = item.get();
    items.push_back(
      std::make_unique< dbscqt::TransactionItem >( dbscqt::createTransactionItemData( transaction, accountBook ) ) );
  }

  return items;
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
