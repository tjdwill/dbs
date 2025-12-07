// dbscqt_accountbookwidget.cpp
#include "dbscqt_accountbookwidget.h"

#include "Qt/6.9.1/gcc_64/include/QtCore/qcontainerfwd.h"

#include <dbsc_accountbook.h>
#include <dbsc_transaction.h>
#include <dbscqt_accountbooktreewidget.h>
#include <dbscqt_accountmodel.h>
#include <dbscqt_displayutil.h>
#include <dbscqt_qobjectdeleteutil.h>
#include <dbscqt_transactionitem.h>

#include <QHeaderView>
#include <QPointer>
#include <QSignalBlocker>
#include <QTableView>
#include <QUuid>
#include <ui_dbscqt_accountbookwidget.h>

#include <memory>

namespace {

/// @return a copy of the input string truncated to two decimal places.
auto truncateDecimalString( QString const& decimalString ) -> QString
{
  int const decimalOffset    = 3; // Not 2; See documentation of QString::first().
  auto const decimalPosition = decimalString.indexOf( '.' );
  QString truncatedString;
  if ( decimalPosition != -1 ) {
    assert( decimalPosition + decimalOffset <= decimalString.size() );
    truncatedString = decimalString.first( decimalPosition + decimalOffset );
  } else {
    truncatedString = decimalString;
  }
  return truncatedString;
}

} // namespace

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
  }

  // Set initial widget state
  clearDisplay();
  handleAccountBookSet( accountBookHandle );
}

dbscqt::AccountBookWidget::~AccountBookWidget() = default;

void dbscqt::AccountBookWidget::handleAccountBookSet( std::shared_ptr< dbsc::AccountBook > accountBookPtr )
{
  assert( mImp->mAccountBookHandle && accountBookPtr );
  mImp->mAccountBookHandle = std::move( accountBookPtr );
  if ( mImp->mTreeWidgetHandle ) {
    mImp->mTreeWidgetHandle.get()->deleteLater();
  }
  createAndInitializeAccountBookTreeWidget();
}

void dbscqt::AccountBookWidget::handleAccountSelected( dbscqt::AccountItem* selectedItem )
{
  auto const& accountItemData = selectedItem->accountItemData();
  mImp->mUi.mBalanceDisplay->setText( truncateDecimalString( accountItemData.mBalance ) );
  mImp->mUi.mDescriptionDisplay->setText( accountItemData.mDescription );
  mImp->mAccountTableView->setModel( selectedItem->accountModel() );
  {
    mImp->mAccountTableView->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
  }
}

void dbscqt::AccountBookWidget::clearDisplay()
{
  mImp->mAccountTableView->setModel( nullptr );
  mImp->mUi.mBalanceDisplay->clear();
  mImp->mUi.mDescriptionDisplay->clear();
}

void dbscqt::AccountBookWidget::createAndInitializeAccountBookTreeWidget()
{
  mImp->mTreeWidgetHandle = QPointer( new dbscqt::AccountBookTreeWidget( mImp->mAccountBookHandle, this ) );
  mImp->mUi.mAccountTreeWidgetContainer->layout()->addWidget( mImp->mTreeWidgetHandle );
  connect( mImp->mTreeWidgetHandle,
           &dbscqt::AccountBookTreeWidget::accountCleared,
           this,
           &dbscqt::AccountBookWidget::clearDisplay );

  connect( mImp->mTreeWidgetHandle,
           &dbscqt::AccountBookTreeWidget::accountSelected,
           this,
           &dbscqt::AccountBookWidget::handleAccountSelected );
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
