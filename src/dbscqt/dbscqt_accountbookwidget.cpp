// dbscqt_accountbookwidget.cpp
#include "dbscqt_accountbookwidget.h"

#include <dbsc_accountbook.h>
#include <dbsc_transaction.h>
#include <dbscqt_accountbooktreewidget.h>
#include <dbscqt_accountmodel.h>
#include <dbscqt_displayutil.h>
#include <dbscqt_openaccountdialog.h>
#include <dbscqt_qobjectdeleteutil.h>
#include <dbscqt_transactiondialog.h>

#include <bsls_assert.h>

#include <QtCore/QPointer>
#include <QtCore/QSettings>
#include <QtCore/QSignalBlocker>
#include <QtCore/QUuid>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableView>
#include <ui_dbscqt_accountbookwidget.h>

#include <memory>
#include <optional>

namespace dbscqt {
namespace {
  static QString const kSplitterStateKey { "state/AccountBookWidgetSplitter" };

  /// @return a copy of the input string truncated to two decimal places.
  auto truncateDecimalString( QString const& decimalString ) -> QString
  {
    int const decimalOffset    = 3; // Not 2; See documentation of QString::first().
    auto const decimalPosition = decimalString.indexOf( '.' );
    QString truncatedString;
    if ( decimalPosition != -1 ) {
      BSLS_ASSERT( decimalPosition + decimalOffset <= decimalString.size() );
      truncatedString = decimalString.first( decimalPosition + decimalOffset );
    } else {
      truncatedString = decimalString;
    }
    return truncatedString;
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
  QPointer< AccountBookTreeWidget > mTreeWidgetHandle;
  QPointer< QTableView > mAccountTableView;
};

dbscqt::AccountBookWidget::AccountBookWidget( std::shared_ptr< dbsc::AccountBook > const& accountBookHandle,
                                              QWidget* parent )
  : QWidget( parent )
  , mImp( std::make_unique< Private >( accountBookHandle ) )
{
  setObjectName( "mAccountBookWidget" );
  mImp->mUi.setupUi( this );
  {
    mImp->mAccountTableView = QPointer( new QTableView() );
    mImp->mUi.mAccountTableViewContainer->layout()->addWidget( mImp->mAccountTableView );
    mImp->mUi.mAccountTreeWidgetContainer->layout()->setContentsMargins( {} );

    auto* splitter = mImp->mUi.mSplitter;
    splitter->setStretchFactor( splitter->indexOf( mImp->mUi.mAccountDisplayWidget ), 1 );
  }

  QObject::connect(
    mImp->mUi.mToggleStatusButton, &QPushButton::clicked, this, &dbscqt::AccountBookWidget::toggleAccountStatus );
  QObject::connect(
    mImp->mUi.mTransactionButton, &QPushButton::clicked, this, &dbscqt::AccountBookWidget::createTransaction );
  QObject::connect(
    mImp->mUi.mCreateAccountButton, &QPushButton::clicked, this, &dbscqt::AccountBookWidget::createAccount );

  // Set initial widget state
  mImp->mUi.mSplitter->restoreState( QSettings().value( dbscqt::kSplitterStateKey ).toByteArray() );
  clearDisplay();
  if ( accountBookHandle ) {
    handleAccountBookSet( accountBookHandle );
  }
}

dbscqt::AccountBookWidget::~AccountBookWidget()
{
  QSettings().setValue( dbscqt::kSplitterStateKey, mImp->mUi.mSplitter->saveState() );
}

void dbscqt::AccountBookWidget::createAccount()
{
  auto* accountCreationDialog = new dbscqt::OpenAccountDialog( this );
  QObject::connect( accountCreationDialog, &QDialog::finished, [this, accountCreationDialog]() {
    if ( accountCreationDialog->result() == QDialog::DialogCode::Accepted ) {
      auto const accountName        = accountCreationDialog->accountName().toStdString();
      auto const accountDescription = accountCreationDialog->accountDescription().toStdString();
      auto const accountId          = mImp->mAccountBookHandle->createAccount( accountName, accountDescription );

      Q_EMIT accountCreated( dbscqt::DisplayUtil::toQUuid( accountId ) );
      Q_EMIT accountBookModified();
    }
    // Clean up dialog
    accountCreationDialog->deleteLater();
  } );

  accountCreationDialog->setModal( true );
  accountCreationDialog->show();
}

void dbscqt::AccountBookWidget::createTransaction()
{
  auto* currentItem = mImp->mTreeWidgetHandle->currentAccountItem();
  BSLS_ASSERT( currentItem );

  std::vector< std::pair< QUuid, QString > > activeAccountData;
  activeAccountData.reserve( mImp->mAccountBookHandle->accountCount() );
  for ( auto* activeItem : mImp->mTreeWidgetHandle->activeAccountItems() ) {
    activeAccountData.push_back( { activeItem->accountId(), activeItem->text( 0 ) } );
  }

  auto* transactionDialog =
    new dbscqt::TransactionDialog( { .mExternalAccountId          = dbscqt::DisplayUtil::externalPartyId(),
                                     .mExternalAccountDisplayName = dbscqt::DisplayUtil::externalPartyLabel(),
                                     .mPrimaryPartyId             = currentItem->accountId(),
                                     .mPrimaryPartyDisplayName    = currentItem->text( 0 ),
                                     .mActiveAccountData          = std::move( activeAccountData ) },
                                   this );
  QObject::connect( transactionDialog, &QDialog::finished, [this, transactionDialog]() {
    if ( transactionDialog->result() == QDialog::DialogCode::Accepted ) {
      auto const otherPartyId      = transactionDialog->otherPartyId();
      auto const primaryPartyId    = transactionDialog->primaryPartyId();
      auto const transactionAmount = transactionDialog->amount();
      auto const transactionNotes  = transactionDialog->notes();

      // Set up backend information
      bool const isExternalParty = otherPartyId == dbscqt::DisplayUtil::externalPartyId();
      auto const dbscOtherPartyIdOpt =
        isExternalParty ? std::nullopt
                        : std::optional< dbsc::UuidString >( dbscqt::DisplayUtil::toDbscUuidString( otherPartyId ) );
      auto const dbscPrimaryPartyId = dbscqt::DisplayUtil::toDbscUuidString( primaryPartyId );
      auto const transactionId =
        mImp->mAccountBookHandle->makeTransaction( dbscqt::DisplayUtil::toDecimal64( transactionAmount ),
                                                   transactionNotes.toStdString(),
                                                   dbscPrimaryPartyId,
                                                   dbscOtherPartyIdOpt );

      // Update GUI
      auto const& primaryPartyTransaction =
        mImp->mAccountBookHandle->account( dbscPrimaryPartyId ).transaction( transactionId );

      auto const primaryPartyTransactionItemData =
        dbscqt::createTransactionItemData( primaryPartyTransaction, *mImp->mAccountBookHandle );

      Q_EMIT transactionComplete( primaryPartyId, primaryPartyTransactionItemData );

      // Update other party in GUI
      if ( !isExternalParty ) {
        auto const& otherPartyTransaction =
          mImp->mAccountBookHandle->account( dbscOtherPartyIdOpt.value() ).transaction( transactionId );

        auto otherPartyTransactionItemData =
          dbscqt::createTransactionItemData( otherPartyTransaction, *mImp->mAccountBookHandle );

        Q_EMIT transactionComplete( otherPartyId, otherPartyTransactionItemData );
      }

      // Update displayed account balance
      handleAccountSelected( mImp->mTreeWidgetHandle->currentAccountItem() );

      Q_EMIT accountBookModified();
    }

    // Clean up dialog
    transactionDialog->deleteLater();
  } );
  transactionDialog->setModal( true );
  transactionDialog->show();
}

void dbscqt::AccountBookWidget::handleAccountBookSet( std::shared_ptr< dbsc::AccountBook > accountBookPtr )
{
  mImp->mAccountBookHandle = std::move( accountBookPtr );
  if ( mImp->mTreeWidgetHandle ) {
    mImp->mTreeWidgetHandle.get()->deleteLater();
  }

  if ( mImp->mAccountBookHandle ) {
    createAndInitializeAccountBookTreeWidget();
  }
}

void dbscqt::AccountBookWidget::handleAccountSelected( dbscqt::AccountItem* selectedItem )
{
  BSLS_ASSERT( selectedItem );
  auto const& accountItemData = selectedItem->accountItemData();
  mImp->mUi.mBalanceDisplay->setText( dbscqt::truncateDecimalString( accountItemData.mBalance ) );
  mImp->mUi.mDescriptionDisplay->setText( accountItemData.mDescription );
  mImp->mAccountTableView->setModel( selectedItem->accountModel() );
  {
    mImp->mAccountTableView->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
  }

  mImp->mUi.mTransactionButton->show();
  mImp->mUi.mTransactionButton->setEnabled( accountItemData.mIsActive );
  mImp->mUi.mToggleStatusButton->show();
  mImp->mUi.mToggleStatusButton->setText( accountItemData.mIsActive ? "Deactivate Account" : "Activate Account" );
}

void dbscqt::AccountBookWidget::toggleAccountStatus()
{
  auto* currentItem = mImp->mTreeWidgetHandle->currentAccountItem();
  BSLS_ASSERT( currentItem );

  // Update backend
  QUuid const accountId = currentItem->accountItemData().mId;
  bool const isActive   = currentItem->accountItemData().mIsActive;
  if ( isActive ) {
    mImp->mAccountBookHandle->deactivate( dbscqt::DisplayUtil::toDbscUuidString( accountId ) );
  } else {
    mImp->mAccountBookHandle->activate( dbscqt::DisplayUtil::toDbscUuidString( accountId ) );
  }

  // Update GUI
  Q_EMIT accountStatusToggled( accountId, !isActive );
  Q_EMIT accountBookModified();
}

void dbscqt::AccountBookWidget::clearDisplay()
{
  mImp->mAccountTableView->setModel( nullptr );
  mImp->mUi.mBalanceDisplay->clear();
  mImp->mUi.mDescriptionDisplay->clear();

  mImp->mUi.mToggleStatusButton->hide();
  mImp->mUi.mTransactionButton->hide();
}

void dbscqt::AccountBookWidget::createAndInitializeAccountBookTreeWidget()
{
  BSLS_ASSERT( mImp->mAccountBookHandle );
  mImp->mTreeWidgetHandle = QPointer( new dbscqt::AccountBookTreeWidget( mImp->mAccountBookHandle, this ) );
  mImp->mUi.mAccountTreeWidgetContainer->layout()->addWidget( mImp->mTreeWidgetHandle );
  QObject::connect( mImp->mTreeWidgetHandle,
                    &dbscqt::AccountBookTreeWidget::accountCleared,
                    this,
                    &dbscqt::AccountBookWidget::clearDisplay );

  QObject::connect( mImp->mTreeWidgetHandle,
                    &dbscqt::AccountBookTreeWidget::accountSelected,
                    this,
                    &dbscqt::AccountBookWidget::handleAccountSelected );

  QObject::connect( this,
                    &dbscqt::AccountBookWidget::accountStatusToggled,
                    mImp->mTreeWidgetHandle,
                    &dbscqt::AccountBookTreeWidget::handleAccountStatusUpdated );
  QObject::connect( this,
                    &dbscqt::AccountBookWidget::transactionComplete,
                    mImp->mTreeWidgetHandle,
                    &dbscqt::AccountBookTreeWidget::handleTransactionMade );
  QObject::connect( this,
                    &dbscqt::AccountBookWidget::accountCreated,
                    mImp->mTreeWidgetHandle,
                    &dbscqt::AccountBookTreeWidget::handleAccountCreated );
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
