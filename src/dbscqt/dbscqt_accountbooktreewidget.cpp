// dbscqt_accountbooktreewidget.cpp
#include "dbscqt_accountbooktreewidget.h"

#include <dbsc_account.h>
#include <dbsc_accountbook.h>
#include <dbsc_transaction.h>
#include <dbscqt_accountmodel.h>
#include <dbscqt_displayutil.h>
#include <dbscqt_qobjectdeleteutil.h>

#include <bsls_assert.h>

#include <stdexcept>

namespace dbscqt {

namespace {
  QString const kActiveAccountsCategoryLabel   = "Active";
  QString const kInactiveAccountsCategoryLabel = "Inactive";
} // namespace

class AccountItem::Private
{
public:
  Private( dbscqt::AccountItemData accountData, dbscqt::AccountModel* accountModelHandle )
    : mAccountItemData( std::move( accountData ) )
    , mAccountModelHandle( accountModelHandle )
  {
  }

  dbscqt::AccountItemData mAccountItemData;
  QPointer< dbscqt::AccountModel > mAccountModelHandle {};
};

class AccountBookTreeWidget::Private
{
public:
  Private( std::shared_ptr< dbsc::AccountBook > const& accountBookHandle )
    : mAccountBookHandle( accountBookHandle )
  {
  }

  std::weak_ptr< dbsc::AccountBook > mAccountBookHandle;
  std::map< QUuid, AccountItem* > mAccountItems;
  QTreeWidgetItem* mActiveAccountsCategoryItem {};
  QTreeWidgetItem* mInactiveAccountsCategoryItem {};
};
} // namespace dbscqt

auto dbscqt::createAccountItemData( dbsc::Account const& account ) -> dbscqt::AccountItemData
{
  return {
    .mName        = QString::fromStdString( account.name() ),
    .mDescription = QString::fromStdString( account.description() ),
    .mBalance     = QString::fromStdString( dbsc::TransactionUtil::currencyToString( account.balance() ) ),
    .mId          = QUuid::fromString( account.id().view() ),
    .mIsActive    = account.isActive(),
  };
}

dbscqt::AccountItem::AccountItem( dbscqt::AccountItemData accountData,
                                  dbscqt::AccountModel* accountModelHandle,
                                  QTreeWidgetItem* parent )
  : QTreeWidgetItem { parent }
  , mImp( std::make_unique< Private >( std::move( accountData ), accountModelHandle ) )
{
}

dbscqt::AccountItem::~AccountItem()
{
  // Delete potentially orphaned AccountModel
  dbscqt::QObjectDeleteUtil::deleteOrphaned( mImp->mAccountModelHandle );
}

auto dbscqt::AccountItem::accountModel() const -> AccountModel*
{
  return mImp->mAccountModelHandle;
}

auto dbscqt::AccountItem::accountItemData() const -> dbscqt::AccountItemData const&
{
  return mImp->mAccountItemData;
}

auto dbscqt::AccountItem::accountId() const -> QUuid
{
  return accountItemData().mId;
}

auto dbscqt::AccountItem::accountItemDataMut() -> dbscqt::AccountItemData&
{
  return mImp->mAccountItemData;
}

//---

dbscqt::AccountBookTreeWidget::AccountBookTreeWidget( std::shared_ptr< dbsc::AccountBook > const& accountBookHandle,
                                                      QWidget* parent )
  : QTreeWidget( parent )
  , mImp( std::make_unique< Private >( accountBookHandle ) )
{
  BSLS_ASSERT( accountBookHandle );

  setHeaderLabel( "Accounts" );
  // Create categorization items. Open accounts should be listed first, so use a name
  // beginning with 'A'.
  mImp->mActiveAccountsCategoryItem   = new QTreeWidgetItem( this, { dbscqt::kActiveAccountsCategoryLabel } );
  mImp->mInactiveAccountsCategoryItem = new QTreeWidgetItem( this, { dbscqt::kInactiveAccountsCategoryLabel } );

  auto const& accountBook = *accountBookHandle;
  for ( auto const& [accountId, account] : accountBook ) {
    auto* accountItem = createAccountItem( account, accountBook );
    if ( accountItem->accountItemData().mIsActive ) {
      mImp->mActiveAccountsCategoryItem->addChild( accountItem );
    } else {
      mImp->mInactiveAccountsCategoryItem->addChild( accountItem );
    }

    mImp->mAccountItems.insert( { dbscqt::DisplayUtil::toQUuid( accountId ), accountItem } );
  }
  mImp->mActiveAccountsCategoryItem->setExpanded( true );
  mImp->mInactiveAccountsCategoryItem->setExpanded( false );

  connect(
    this, &QTreeWidget::currentItemChanged, this, [this]( QTreeWidgetItem* current, QTreeWidgetItem* /*previous*/ ) {
      if ( isAccountItemInThisTree( current ) ) {
        Q_EMIT accountSelected( dynamic_cast< dbscqt::AccountItem* >( current ) );
      } else {
        Q_EMIT accountCleared();
      }
    } );
}

dbscqt::AccountBookTreeWidget::~AccountBookTreeWidget() = default;

auto dbscqt::AccountBookTreeWidget::accountItem( QUuid const accountId ) const -> dbscqt::AccountItem*
{
  return mImp->mAccountItems.at( accountId );
}

auto dbscqt::AccountBookTreeWidget::accountItemData( QUuid const accountId ) const -> dbscqt::AccountItemData const&
{
  return mImp->mAccountItems.at( accountId )->accountItemData();
}

auto dbscqt::AccountBookTreeWidget::accountModel( QUuid const accountId ) const -> dbscqt::AccountModel*
{
  return mImp->mAccountItems.at( accountId )->accountModel();
}

auto dbscqt::AccountBookTreeWidget::accountItemDataMut( QUuid const accountId ) -> dbscqt::AccountItemData&
{
  return mImp->mAccountItems.at( accountId )->accountItemDataMut();
}

auto dbscqt::AccountBookTreeWidget::addAccountItem( AccountItem* accountItemCandidate ) -> bool
{
  if ( mImp->mAccountItems.contains( accountItemCandidate->accountId() ) ) {
    return false;
  }
  auto [_, insertSuccessful] =
    mImp->mAccountItems.insert( { accountItemCandidate->accountId(), accountItemCandidate } );

  return insertSuccessful;
}

void dbscqt::AccountBookTreeWidget::handleAccountCreated( QUuid accountId )
{
  auto accountBookHandle = mImp->mAccountBookHandle.lock();
  BSLS_ASSERT( accountBookHandle );
  if ( !addAccountItem( createAccountItem(
         accountBookHandle->account( dbscqt::DisplayUtil::toDbscUuidString( accountId ) ), *accountBookHandle ) ) ) {
    throw std::runtime_error( "Could not create the account." );
  }
}

void dbscqt::AccountBookTreeWidget::handleAccountStatusUpdated( QUuid const accountId, bool const isActive )
{
  auto* accountItemHandle                           = accountItem( accountId );
  accountItemHandle->accountItemDataMut().mIsActive = isActive;
  accountItemHandle->parent()->removeChild( accountItemHandle );
  categoryItem( isActive )->addChild( accountItemHandle );
}

void dbscqt::AccountBookTreeWidget::handleTransactionMade( QUuid const accountId,
                                                           dbscqt::TransactionItem* const transactionItem )
{
  accountModel( accountId )->addTransactionItem( transactionItem );
}

auto dbscqt::AccountBookTreeWidget::isMemberOfThisTree( QTreeWidgetItem* itemCandidate ) -> bool
{
  return itemCandidate and itemCandidate->treeWidget() == this;
}

auto dbscqt::AccountBookTreeWidget::isAccountItemInThisTree( QTreeWidgetItem* itemCandidate ) -> bool
{
  return isMemberOfThisTree( itemCandidate ) and static_cast< bool >( dynamic_cast< AccountItem* >( itemCandidate ) );
}

auto dbscqt::AccountBookTreeWidget::createAccountItem( dbsc::Account const& account,
                                                       dbsc::AccountBook const& accountBook ) -> dbscqt::AccountItem*
{
  auto accountItemData = dbscqt::createAccountItemData( account );
  auto* accountModel   = new dbscqt::AccountModel( dbscqt::createTransactionItems( account, accountBook ) );

  auto* accountItem = new AccountItem { std::move( accountItemData ), accountModel, nullptr };
  accountItem->setText( 0,
                        dbscqt::DisplayUtil::accountNameWithShortenedUuid( accountItem->accountId(),
                                                                           accountItem->accountItemData().mName ) );

  return accountItem;
}

auto dbscqt::AccountBookTreeWidget::categoryItem( bool const accountIsActive ) const -> QTreeWidgetItem*
{
  auto* item = accountIsActive ? mImp->mActiveAccountsCategoryItem : mImp->mInactiveAccountsCategoryItem;

  return item;
}
