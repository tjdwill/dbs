// dbsqt_accountmodel.cpp
#include "dbsqt_accountmodel.h"

#include "Qt/6.9.1/gcc_64/include/QtCore/qabstractitemmodel.h"
#include "Qt/6.9.1/gcc_64/include/QtCore/qnamespace.h"

namespace dbsqt {

namespace {

  enum class AccountModelColumnType
  {
    kDateTime,
    kAmount,
    kTransactionId,
    kOtherPartyId,
    kNotes,
    // PRIVATE
    kVariantCount,
  };

  constexpr auto accountModelColumnIndex( AccountModelColumnType type ) -> int
  {
    return static_cast< int >( type );
  }
} // namespace

TransactionItem::TransactionItem( QUuid const& transactionId,
                                  QUuid const& otherPartyId,
                                  QString const& transactionAmount,
                                  QDateTime const& timeStamp,
                                  QString const& notes )
  : mTimeStamp( timeStamp )
  , mTransactionAmount( transactionAmount )
  , mNotes( notes )
  , mTransactionId( transactionId )
  , mOtherPartyId( otherPartyId )
{
}

class AccountModel::Private
{
public:
  Private( std::vector< std::unique_ptr< TransactionItem > > items,
           QUuid id,
           QString const& name,
           QString const& description,
           QString const& balance,
           bool isOpen )
    : mItems( std::move( items ) )
    , mName( name )
    , mDescription( description )
    , mBalance( balance )
    , mId( id )
    , mIsOpen( isOpen )
  {
  }

  std::vector< std::unique_ptr< TransactionItem > > mItems;
  QString mName;
  QString mDescription;
  QString mBalance;
  QUuid mId;
  bool mIsOpen;
};

AccountModel::AccountModel( std::vector< std::unique_ptr< TransactionItem > > transactionItems,
                            QUuid id,
                            QString const& name,
                            QString const& description,
                            QString const& balance,
                            bool isOpen,
                            QObject* parent )
  : QAbstractTableModel( parent )
  , mImp( std::make_unique< Private >( std::move( transactionItems ), id, name, description, balance, isOpen ) )
{
}

AccountModel::~AccountModel() = default;

auto AccountModel::rowCount( QModelIndex const& /* parent */ ) const -> int
{
  return static_cast< int >( mImp->mItems.size() );
}

auto AccountModel::columnCount( QModelIndex const& /* parent */ ) const -> int
{
  return accountModelColumnIndex( AccountModelColumnType::kVariantCount );
}

auto AccountModel::data( QModelIndex const& index, int role ) const -> QVariant
{
  int const row    = index.row();
  int const column = index.column();

  if ( !index.isValid() || row < 0 || row > rowCount() || column < 0 || column > columnCount() ) {
    return QVariant();
  }

  if ( role == Qt::DisplayRole ) {
    auto const& item = mImp->mItems.at( row );
    switch ( column ) {
      case accountModelColumnIndex( AccountModelColumnType::kTransactionId ):
        return item->transactionId().toString( QUuid::WithoutBraces );
      case accountModelColumnIndex( AccountModelColumnType::kAmount ):
        return item->amount();
      case accountModelColumnIndex( AccountModelColumnType::kDateTime ):
        return item->timeStamp().toString( Qt::ISODateWithMs );
      case accountModelColumnIndex( AccountModelColumnType::kOtherPartyId ):
        return item->otherPartyId().toString( QUuid::WithoutBraces );
      case accountModelColumnIndex( AccountModelColumnType::kNotes ):
        return item->notes();
    }
  }
  return QVariant();
}

auto AccountModel::headerData( int section, Qt::Orientation orientation, int role ) const -> QVariant
{
  if ( orientation == Qt::Horizontal && role == Qt::DisplayRole && section >= 0 && section < columnCount() ) {
    switch ( section ) {
      case accountModelColumnIndex( AccountModelColumnType::kTransactionId ):
        return "Transaction Id";
      case accountModelColumnIndex( AccountModelColumnType::kAmount ):
        return "Amount ($)";
      case accountModelColumnIndex( AccountModelColumnType::kDateTime ):
        return "Time";
      case accountModelColumnIndex( AccountModelColumnType::kOtherPartyId ):
        return "Other Party Id";
      case accountModelColumnIndex( AccountModelColumnType::kNotes ):
        return "Notes";
    }
  }
  return QVariant();
}

} // namespace dbsqt

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
