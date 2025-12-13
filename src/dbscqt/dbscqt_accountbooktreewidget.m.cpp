// dbscqt_accountbooktreewidget.m.cpp

#include <dbsc_account.h>
#include <dbsc_accountbook.h>
#include <dbscqt_accountbooktreewidget.h>
#include <dbscqt_accountmodel.h>
#include <dbscqt_displayutil.h>
#include <dbscqt_transactionitem.h>

#include <bdldfp_decimal.h>
#include <bsls_assert.h>

#include <QtCore/QPointer>
#include <QtCore/QScopedPointer>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

namespace {

class AccountBookTreeWidgetTester : public QWidget
{
  Q_OBJECT
public:
  AccountBookTreeWidgetTester( QWidget* parent = nullptr );

public Q_SLOTS:
  void clearDisplay();
  void handleAccountSelected( dbscqt::AccountItem* item );
  void handleStatusToggleClicked();

Q_SIGNALS:

  void accountCreated( QUuid accountId );
  void accountStatusUpdated( QUuid accountId, bool isActive );
  void transactionMade( QUuid accountId, dbscqt::TransactionItem* transactionData );

private:
  void createAccount();
  void transact( QUuid accountId );

  std::shared_ptr< dbsc::AccountBook > mAccountBookHandle {};
  QPointer< QTableView > mTableView {};
  QPointer< dbscqt::AccountBookTreeWidget > mAccountBookTreeWidget {};
  QPointer< QPushButton > mStatusToggleButton {};
  QPointer< QPushButton > mAddAccountButton {};
  QPointer< QPushButton > mTransactionButton {};
};

AccountBookTreeWidgetTester::AccountBookTreeWidgetTester( QWidget* parent )
  : QWidget( parent )
  , mAccountBookHandle( std::make_unique< dbsc::AccountBook >( "tjdwill" ) )
{
  setObjectName( "AccountBookTreeWidgetTester" );
  auto* mainLayout              = new QHBoxLayout( this );
  auto* accountOperationsWidget = new QWidget();
  {
    mainLayout->addWidget( accountOperationsWidget );
    auto* accountOperationsLayout = new QGridLayout( accountOperationsWidget );

    mAccountBookTreeWidget = QPointer( new dbscqt::AccountBookTreeWidget( mAccountBookHandle ) );
    {
      QObject::connect( mAccountBookTreeWidget,
                        &dbscqt::AccountBookTreeWidget::accountSelected,
                        this,
                        &AccountBookTreeWidgetTester::handleAccountSelected );
      QObject::connect( mAccountBookTreeWidget,
                        &dbscqt::AccountBookTreeWidget::accountCleared,
                        this,
                        &AccountBookTreeWidgetTester::clearDisplay );

      QObject::connect( this,
                        &AccountBookTreeWidgetTester::accountCreated,
                        mAccountBookTreeWidget,
                        &dbscqt::AccountBookTreeWidget::handleAccountCreated );
      QObject::connect( this,
                        &AccountBookTreeWidgetTester::accountStatusUpdated,
                        mAccountBookTreeWidget,
                        &dbscqt::AccountBookTreeWidget::handleAccountStatusUpdated );
      QObject::connect( this,
                        &AccountBookTreeWidgetTester::transactionMade,
                        mAccountBookTreeWidget,
                        &dbscqt::AccountBookTreeWidget::handleTransactionMade );
    }
    mStatusToggleButton = QPointer( new QPushButton( "Toggle Account Status" ) );
    {
      mStatusToggleButton->setObjectName( "mStatusToggleButton" );
      mStatusToggleButton->setEnabled( false );
      auto* toggleStatusAction = new QAction( "Toggle Account Status" );
      mStatusToggleButton->addAction( toggleStatusAction );

      QObject::connect( toggleStatusAction, &QAction::triggered, mStatusToggleButton, &QPushButton::clicked );
      QObject::connect(
        mStatusToggleButton, &QPushButton::clicked, this, &AccountBookTreeWidgetTester::handleStatusToggleClicked );
    }

    mAddAccountButton = QPointer( new QPushButton( "Add Account" ) );
    {
      mAddAccountButton->setObjectName( "mAddAccountButton" );
      auto* addAccountAction = new QAction( "Add Account" );
      mAddAccountButton->addAction( addAccountAction );

      QObject::connect( addAccountAction, &QAction::triggered, mAddAccountButton, &QPushButton::clicked );
      QObject::connect( mAddAccountButton, &QPushButton::clicked, this, &AccountBookTreeWidgetTester::createAccount );
    }

    mTransactionButton = QPointer( new QPushButton( "Make Transaction" ) );
    {
      mTransactionButton->setObjectName( "mTransactionButton" );
      auto* makeTransactionAction = new QAction( "Make Transaction" );
      mTransactionButton->addAction( makeTransactionAction );

      QObject::connect( makeTransactionAction, &QAction::triggered, mTransactionButton, &QPushButton::clicked );
      QObject::connect( mTransactionButton, &QPushButton::clicked, this, [this]() {
        if ( auto* accountItem = dynamic_cast< dbscqt::AccountItem* >( mAccountBookTreeWidget->currentItem() ) ) {
          transact( accountItem->accountItemData().mId );
        }
      } );
    }
    accountOperationsLayout->addWidget( mAccountBookTreeWidget, 0, 0, 1, 2 );
    accountOperationsLayout->addWidget( mAddAccountButton, 1, 0, 1, 2 );
    accountOperationsLayout->addWidget( mStatusToggleButton, 2, 0, 1, 1 );
    accountOperationsLayout->addWidget( mTransactionButton, 2, 1, 1, 1 );
  }

  mTableView = QPointer( new QTableView() );
  mainLayout->addWidget( mTableView );
  mainLayout->setStretchFactor( mTableView, 1 );
}

void AccountBookTreeWidgetTester::clearDisplay()
{
  mTableView->setModel( nullptr );
  mStatusToggleButton->setEnabled( false );
  mTransactionButton->setEnabled( false );
}

void AccountBookTreeWidgetTester::handleAccountSelected( dbscqt::AccountItem* item )
{
  mTableView->setModel( item->accountModel() );
  mTableView->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );

  bool const isActive { item->accountItemData().mIsActive };
  mStatusToggleButton->setEnabled( true );
  isActive ? mStatusToggleButton->setText( "Deactivate" ) : mStatusToggleButton->setText( "Activate" );

  mTransactionButton->setEnabled( isActive );
}

void AccountBookTreeWidgetTester::handleStatusToggleClicked()
{
  if ( auto* accountItem = dynamic_cast< dbscqt::AccountItem* >( mAccountBookTreeWidget->currentItem() ) ) {
    auto const dbscUuid = dbscqt::DisplayUtil::toDbscUuidString( accountItem->accountId() );
    bool const isActive = accountItem->accountItemData().mIsActive;
    isActive ? mAccountBookHandle->deactivate( dbscUuid ) : mAccountBookHandle->activate( dbscUuid );
    mTransactionButton->setEnabled( isActive );

    Q_EMIT accountStatusUpdated( accountItem->accountId(), !isActive );
  }
}

void AccountBookTreeWidgetTester::createAccount()
{
  static int sNumber { 0 };
  std::ostringstream oss {};
  oss << "account" << sNumber;
  ++sNumber;

  auto const accountId = mAccountBookHandle->createAccount( oss.str(), "" );

  Q_EMIT accountCreated( dbscqt::DisplayUtil::toQUuid( accountId ) );
}

void AccountBookTreeWidgetTester::transact( QUuid accountId )
{
  using namespace BloombergLP::bdldfp::DecimalLiterals;
  auto const amount = []() -> BloombergLP::bdldfp::Decimal64 {
    static BloombergLP::bdldfp::Decimal64 sTransactionAmount { "1.10"_d64 };
    static int sCallCount        = 0;
    constexpr int kCallThreshold = 3;
    ++sCallCount;
    return sCallCount % kCallThreshold == 0 ? -kCallThreshold * sTransactionAmount : sTransactionAmount;
  };

  auto const dbscUuid = dbscqt::DisplayUtil::toDbscUuidString( accountId );
  BSLS_ASSERT( mAccountBookHandle->account( dbscUuid ).isActive() );
  auto const dbscTransactionId = mAccountBookHandle->makeTransaction( amount(), "", dbscUuid, {} );

  // In a real implementation, emit the transactionMade signal for both accounts
  // (Assuming two accounts).
  Q_EMIT transactionMade(
    accountId,
    new dbscqt::TransactionItem( dbscqt::createTransactionItemData(
      mAccountBookHandle->account( dbscUuid ).transaction( dbscTransactionId ), *mAccountBookHandle ) ) );
}
} // namespace

int main( int argc, char* argv[] )
{
  QApplication app { argc, argv };

  QScopedPointer< AccountBookTreeWidgetTester > tester { new AccountBookTreeWidgetTester( nullptr ) };
  tester->show();

  return app.exec();
}

#include <dbscqt_accountbooktreewidget.m.moc>
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
