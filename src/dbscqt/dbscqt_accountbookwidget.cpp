// dbscqt_accountbookwidget.cpp
#include "dbscqt_accountbookwidget.h"

#include "dbsc_accountbook.h"

#include <dbsc_transaction.h>
#include <dbscqt_accountmodel.h>

#include <QPointer>
#include <QTableView>
#include <ui_dbscqt_accountbookwidget.h>

#include <memory>
#include <ranges>

namespace dbscqt {
namespace {
  static auto createAccountModelData( dbsc::Account const& account ) -> dbscqt::AccountModelData
  {
    return {
      .mName        = QString::fromStdString( account.name() ),
      .mDescription = QString::fromStdString( account.description() ),
      .mBalance     = QString::fromStdString( dbsc::TransactionUtil::currencyAsString( account.balance() ) ),
      .mId          = QUuid::fromString( account.id().view() ),
      .mIsOpen      = account.isOpen(),
    };
  }

  static auto createTransactionItemData( dbsc::Transaction const& transaction, dbsc::AccountBook const& accountBook )
    -> dbscqt::TransactionItemData
  {

    QDateTime const timestamp = QDateTime::fromStdTimePoint(
      std::chrono::time_point_cast< std::chrono::milliseconds >( transaction.timeStamp() ) );
    QString const transactionAmount =
      QString::fromStdString( dbsc::TransactionUtil::currencyAsString( transaction.amount() ) );
    QUuid const otherPartyId = QUuid::fromString( transaction.otherPartyId().view() );
    QString const otherPartyDisplayName =
      otherPartyId.isNull() ? "" : QString::fromStdString( accountBook.account( transaction.otherPartyId() ).name() );

    return {
      .mTimeStamp             = timestamp,
      .mTransactionAmount     = transactionAmount,
      .mNotes                 = QString::fromStdString( transaction.notes() ),
      .mOtherPartyAccountName = otherPartyDisplayName,
      .mTransactionId         = QUuid::fromString( transaction.transactionId().view() ),
      .mOtherPartyId          = otherPartyId,
    };
  };

  /// @return a sequence of transaction items sorted in descending date order.
  static auto createTransactionItems( dbsc::Account const& account, dbsc::AccountBook const& accountBook )
    -> std::vector< std::unique_ptr< dbscqt::TransactionItem > >
  {
    auto transactionsSortedByDescendingDate =
      account | std::views::transform( []( auto const& transaction ) { return std::cref( transaction ); } )
      | std::ranges::to< std::vector >();
    std::ranges::sort( transactionsSortedByDescendingDate, std::greater<>(), []( auto&& item ) -> dbsc::TimeStamp {
      auto const& [_, transaction] = item.get();
      return transaction.timeStamp();
    } );

    std::vector< std::unique_ptr< dbscqt::TransactionItem > > items;
    items.reserve( account.transactionCount() );
    for ( auto const& item : transactionsSortedByDescendingDate ) {
      auto const& [id, transaction] = item.get();
      items.push_back(
        std::make_unique< dbscqt::TransactionItem >( createTransactionItemData( transaction, accountBook ) ) );
    }

    return items;
  }

  struct StoredAccountDisplayData
  {
    dbscqt::AccountModelData mAccountModelData;
    QPointer< dbscqt::AccountModel > mAccountModelPtr {};
  };

  auto toQUuid( dbsc::UuidString const& uuidString ) -> QUuid
  {
    return QUuid::fromString( uuidString.view() );
  }

  template< typename T >
  void deleteOrphanedPointerObject( QPointer< T >& pointer )
  {
    if ( !pointer.isNull() && !pointer->parent() ) {
      delete pointer.get();
    };
  }

  template< typename T >
  void deleteNonNullPointerObject( QPointer< T >& pointer )
  {
    if ( !pointer.isNull() ) {
      delete pointer.get();
    }
  }
} // namespace
} // namespace dbscqt

class dbscqt::AccountBookWidget::Private
{
public:
  Private( std::shared_ptr< dbsc::AccountBook > const& accountBook )
    : mAccountBookPtr( accountBook )
  {
  }

  ::Ui::AccountBookWidget mUi {};
  std::shared_ptr< dbsc::AccountBook > mAccountBookPtr;
  std::map< QUuid, dbscqt::StoredAccountDisplayData > mDisplayDataMap;
};

dbscqt::AccountBookWidget::AccountBookWidget( std::shared_ptr< dbsc::AccountBook > const& accountBook, QWidget* parent )
  : QWidget( parent )
  , mImp( std::make_unique< Private >( accountBook ) )
{
  mImp->mUi.setupUi( this );

  handleAccountBookChanged( accountBook );
}

dbscqt::AccountBookWidget::~AccountBookWidget()
{
  // Delete any orphaned AccountModels
  for ( auto& [_, storedDisplayData] : mImp->mDisplayDataMap ) {
    dbscqt::deleteOrphanedPointerObject( storedDisplayData.mAccountModelPtr );
  }
}

void dbscqt::AccountBookWidget::deleteAccountModels()
{
  for ( auto& [_, storedDisplayData] : mImp->mDisplayDataMap ) {
    dbscqt::deleteNonNullPointerObject( storedDisplayData.mAccountModelPtr );
  }
}

void dbscqt::AccountBookWidget::handleAccountBookChanged( std::shared_ptr< dbsc::AccountBook > accountBookPtr )
{
  deleteAccountModels();
  mImp->mAccountBookPtr = accountBookPtr;

  mImp->mDisplayDataMap.clear();
  for ( auto const& [id, account] : *mImp->mAccountBookPtr ) {
    QUuid const accountId = toQUuid( id );

    mImp->mDisplayDataMap.insert( {
      accountId,
      dbscqt::StoredAccountDisplayData {
                                        .mAccountModelData = createAccountModelData( account ),
                                        .mAccountModelPtr =
          new dbscqt::AccountModel( dbscqt::createAccountModelData( account ),
                                        dbscqt::createTransactionItems( account, *mImp->mAccountBookPtr ), ),
                                        },
    } );
  }

  refreshAccountComboBox();
}

/// Precondition(s):
///     - The account book map has valid data.
///     - The account book pointer is non-null
void dbscqt::AccountBookWidget::refreshAccountComboBox()
{
  mImp->mUi.mAccountSelectionBox->clear();
  assert( mImp->mAccountBookPtr );

  for ( auto const& [accountId, displayData] : mImp->mDisplayDataMap ) {
    auto const accountModel = mImp->mDisplayDataMap[accountId].mAccountModelPtr;
    mImp->mUi.mAccountSelectionBox->addItem( accountModel->accountDisplayText(),
                                             QVariant( accountModel->accountId() ) );
  }
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
