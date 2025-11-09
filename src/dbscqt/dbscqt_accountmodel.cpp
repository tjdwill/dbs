// dbscqt_accountmodel.cpp
#include "dbscqt_accountmodel.h"

#include <dbsutl_helpers.h>

#include <QTimeZone>

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
    auto const& item = mImp->mItems.at( row );
    switch ( column ) {
      case dbsutl::enumAsIntegral( AccountModelColumnType::kTransactionId ):
        return item->transactionId().toString( QUuid::WithoutBraces );
      case dbsutl::enumAsIntegral( AccountModelColumnType::kAmount ):
        return item->amount();
      case dbsutl::enumAsIntegral( AccountModelColumnType::kDateTime ):
        /// TODO: Link this logic and the header logic via a QSettings entry
        return item->timeStamp().toLocalTime().toString( "yyyy-MM-dd hh:mm:ss.zzz" );
      case dbsutl::enumAsIntegral( AccountModelColumnType::kOtherPartyIdentifier ):
        return item->otherPartyDisplayName();
      case dbsutl::enumAsIntegral( AccountModelColumnType::kNotes ):
        return item->notes();
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
      case dbsutl::enumAsIntegral( AccountModelColumnType::kDateTime ):
        return QString( "Time (%1)" ).arg( QTimeZone::systemTimeZone().displayName( QDateTime::currentDateTime() ) );
      case dbsutl::enumAsIntegral( AccountModelColumnType::kOtherPartyIdentifier ):
        return "Other Party";
      case dbsutl::enumAsIntegral( AccountModelColumnType::kNotes ):
        return "Notes";
    }
  }
  return QVariant();
}

dbscqt::TransactionItem::TransactionItem( dbscqt::TransactionItemData const& transactionData )
  : mData( transactionData )
{
}

auto dbscqt::TransactionItem::otherPartyDisplayName() const -> QString
{
  return mData.mOtherPartyId == QUuid()
         ? "External"
         : dbscqt::createDisplayText( mData.mOtherPartyId, mData.mOtherPartyAccountName );
}

auto dbscqt::createDisplayText( QUuid id, QString const& name ) -> QString
{
  return QString( "%1 (%2)" ).arg( name ).arg( id.toString( QUuid::WithoutBraces ).split( '-' ).front() );
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
