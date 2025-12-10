// dbscqt_accountmodel.cpp
#include "dbscqt_accountmodel.h"

#include <dbscqt_transactionitem.h>
#include <dbsutl_helpers.h>

#include <bsls_assert.h>

#include <QTimeZone>

#include <memory>
#include <stdexcept>

namespace dbscqt {

namespace {

  enum class AccountModelColumnType
  {
    kDateTime,
    kAmount,
    kOtherPartyIdentifier,
    kNotes,
    kTransactionId,
    // PRIVATE
    kVariantCount,
  };

} // namespace

class AccountModel::Private
{
public:
  Private( std::vector< std::unique_ptr< TransactionItem > > items )
    : mItems( std::move( items ) )
  {
  }

  std::vector< std::unique_ptr< TransactionItem > > mItems;
};

} // namespace dbscqt

dbscqt::AccountModel::AccountModel( std::vector< std::unique_ptr< TransactionItem > > transactionItems,
                                    QObject* parent )
  : QAbstractTableModel( parent )
  , mImp( std::make_unique< Private >( std::move( transactionItems ) ) )
{
}

dbscqt::AccountModel::~AccountModel() = default;

auto dbscqt::AccountModel::rowCount( QModelIndex const& /* parent */ ) const -> int
{
  return static_cast< int >( mImp->mItems.size() );
}

auto dbscqt::AccountModel::columnCount( QModelIndex const& /* parent */ ) const -> int
{
  return dbsutl::enumAsIntegral( AccountModelColumnType::kVariantCount );
}

auto dbscqt::AccountModel::data( QModelIndex const& index, int role ) const -> QVariant
{
  int const row    = index.row();
  int const column = index.column();

  if ( !index.isValid() || row < 0 || row > rowCount() || column < 0 || column > columnCount() ) {
    return QVariant();
  }

  if ( role == Qt::DisplayRole ) {
    // The items vector is sorted such that the most recent transaction is at the back.
    // We present items such that the most recent is listed first.
    int const itemIndex = ( rowCount() - 1 ) - row;
    auto const& item    = mImp->mItems.at( itemIndex );
    switch ( column ) {
      case dbsutl::enumAsIntegral( AccountModelColumnType::kTransactionId ):
        return item->transactionId().toString( QUuid::WithoutBraces );
      case dbsutl::enumAsIntegral( AccountModelColumnType::kAmount ):
        return item->amount();
      case dbsutl::enumAsIntegral( AccountModelColumnType::kDateTime ): {
        auto const timeStamp         = item->timeStamp();
        auto const localTime         = timeStamp.toLocalTime();
        auto const timeDisplayString = localTime.toString( "yyyy-MM-dd hh:mm:ss.zzz" );
        return timeDisplayString;
      }
      case dbsutl::enumAsIntegral( AccountModelColumnType::kOtherPartyIdentifier ):
        return item->otherPartyDisplayName();
      case dbsutl::enumAsIntegral( AccountModelColumnType::kNotes ):
        return item->notes();
      default:
        throw std::invalid_argument( "Unhandled enum variant" );
    }
  }
  return QVariant();
}

auto dbscqt::AccountModel::headerData( int section, Qt::Orientation orientation, int role ) const -> QVariant
{
  if ( orientation == Qt::Horizontal && role == Qt::DisplayRole && section >= 0 && section < columnCount() ) {
    switch ( section ) {
      case dbsutl::enumAsIntegral( AccountModelColumnType::kTransactionId ):
        return "Transaction Id";
      case dbsutl::enumAsIntegral( AccountModelColumnType::kAmount ):
        return "Amount ($)";
      case dbsutl::enumAsIntegral( AccountModelColumnType::kDateTime ): {
        auto const timezoneString = QTimeZone::systemTimeZone().displayName( QDateTime::currentDateTime() );
        return QString( "Time (%1)" ).arg( timezoneString );
      }
      case dbsutl::enumAsIntegral( AccountModelColumnType::kOtherPartyIdentifier ):
        return "Other Party";
      case dbsutl::enumAsIntegral( AccountModelColumnType::kNotes ):
        return "Notes";
      default:
        throw std::invalid_argument( "Unhandled enum variant" );
    }
  }
  return QVariant();
}

void dbscqt::AccountModel::addTransactionItem( dbscqt::TransactionItem* const transactionItemPtr )
{
  // Check greater than or equal to in case the timestamps are truncated to the
  // same value when converting from nanosecond precision to millisecond.
  BSLS_ASSERT( ( rowCount() > 0 ? transactionItemPtr->timeStamp() >= mImp->mItems.back()->timeStamp() : true ) );

  beginInsertRows( QModelIndex(), 0, 0 );
  {
    mImp->mItems.emplace_back( transactionItemPtr );
  }
  endInsertRows();
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
