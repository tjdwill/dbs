// dbscqt_accountbookwidget.cpp
#include "dbscqt_accountbookwidget.h"

#include <dbsc_transaction.h>
#include <dbscqt_accountmodel.h>
#include <dbscqt_qobjectdeleteutil.h>
#include <dbscqt_transactionitem.h>

#include <QHeaderView>
#include <QPointer>
#include <QSignalBlocker>
#include <QTableView>
#include <ui_dbscqt_accountbookwidget.h>

#include <memory>
#include <ranges>

namespace dbscqt {
namespace {
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
      items.push_back( std::make_unique< dbscqt::TransactionItem >(
        dbscqt::DisplayUtil::createTransactionItemData( transaction, accountBook ) ) );
    }

    return items;
  }

  struct StoredAccountDisplayData
  {
    dbscqt::AccountModelData mAccountModelData;
    QPointer< dbscqt::AccountModel > mAccountModelHandle {};
  };

  auto toQUuid( dbsc::UuidString const& uuidString ) -> QUuid
  {
    return QUuid::fromString( uuidString.toStdString() );
  }

} // namespace
} // namespace dbscqt

class dbscqt::AccountBookWidget::Private
{
public:
  Private( std::shared_ptr< dbsc::AccountBook > const& accountBookHandle )
    : mAccountBookHandle( accountBookHandle )
  {
  }

  ::Ui::AccountBookWidget mUi {};
  std::shared_ptr< dbsc::AccountBook > mAccountBookHandle;
  std::map< QUuid, dbscqt::StoredAccountDisplayData > mDisplayDataMap;
  QTableView* mAccountTableView {};
};

dbscqt::AccountBookWidget::AccountBookWidget( std::shared_ptr< dbsc::AccountBook > const& accountBookHandle,
                                              QWidget* parent )
  : QWidget( parent )
  , mImp( std::make_unique< Private >( accountBookHandle ) )
{
  mImp->mUi.setupUi( this );
  {
    mImp->mUi.mAccountSelectionBox->setInsertPolicy( QComboBox::InsertAlphabetically );
    mImp->mAccountTableView = new QTableView();
    mImp->mUi.mAccountTableViewContainer->layout()->addWidget( mImp->mAccountTableView );
  }

  connect( mImp->mUi.mAccountSelectionBox, &QComboBox::currentIndexChanged, this, [this]( int index ) {
    if ( index > -1 ) {
      handleAccountSelected( mImp->mUi.mAccountSelectionBox->itemData( index ).toUuid() );
    } else {
      clearDisplay();
    }
  } );

  handleAccountBookSet( accountBookHandle );
}

dbscqt::AccountBookWidget::~AccountBookWidget()
{
  // Delete any orphaned AccountModels
  for ( auto& [_, storedDisplayData] : mImp->mDisplayDataMap ) {
    dbscqt::QObjectDeleteUtil::deleteOrphaned( storedDisplayData.mAccountModelHandle );
  }
}

void dbscqt::AccountBookWidget::handleAccountBookSet( std::shared_ptr< dbsc::AccountBook > accountBookPtr )
{
  assert( mImp->mAccountBookHandle && accountBookPtr );
  deleteAccountModels();
  mImp->mDisplayDataMap.clear();
  mImp->mAccountBookHandle = std::move( accountBookPtr );
  auto const& accountBook  = *mImp->mAccountBookHandle;
  for ( auto const& [id, account] : accountBook ) {
    QUuid const accountId         = toQUuid( id );
    auto const accountModelData   = dbscqt::DisplayUtil::createAccountModelData( account );
    auto const accountDisplayData = dbscqt::StoredAccountDisplayData {
      .mAccountModelData = accountModelData,
      .mAccountModelHandle =
        new dbscqt::AccountModel( dbscqt::createTransactionItems( account, *mImp->mAccountBookHandle ), nullptr )
    };
    mImp->mDisplayDataMap.insert( { accountId, std::move( accountDisplayData ) } );
  }

  refreshAccountComboBox();
}

void dbscqt::AccountBookWidget::handleAccountSelected( QUuid accountId )
{
  auto const& displayData = mImp->mDisplayDataMap.at( accountId );
  mImp->mUi.mBalanceDisplay->setText( displayData.mAccountModelData.mBalance );
  mImp->mUi.mDescriptionDisplay->setText( displayData.mAccountModelData.mDescription );
  mImp->mAccountTableView->setModel( displayData.mAccountModelHandle );
  {
    mImp->mAccountTableView->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
  }
}

void dbscqt::AccountBookWidget::clearDisplay()
{
  QSignalBlocker sbAccountSelectionBox { mImp->mUi.mAccountSelectionBox };
  mImp->mUi.mAccountSelectionBox->clear();
  mImp->mAccountTableView->setModel( nullptr );
  mImp->mUi.mBalanceDisplay->clear();
  mImp->mUi.mDescriptionDisplay->clear();
}

void dbscqt::AccountBookWidget::deleteAccountModels()
{
  for ( auto& [_, storedDisplayData] : mImp->mDisplayDataMap ) {
    dbscqt::QObjectDeleteUtil::deleteUnchecked( storedDisplayData.mAccountModelHandle );
  }
}

/// Precondition(s):
///     - The account book map has valid data.
///     - The account book pointer is non-null
void dbscqt::AccountBookWidget::refreshAccountComboBox()
{
  mImp->mUi.mAccountSelectionBox->clear();
  assert( mImp->mAccountBookHandle );

  // Display open accounts.
  for ( auto const& [accountId, displayData] : mImp->mDisplayDataMap ) {
    auto const& accountModelData = mImp->mDisplayDataMap[accountId].mAccountModelData;
    if ( accountModelData.mIsOpen ) {
      mImp->mUi.mAccountSelectionBox->addItem(
        dbscqt::DisplayUtil::accountNameWithShortenedUuid( accountModelData.mId, accountModelData.mName ),
        QVariant( accountModelData.mId ) );
    }
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
