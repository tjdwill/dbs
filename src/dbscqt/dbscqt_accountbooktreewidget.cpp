// dbscqt_accountbooktreewidget.cpp
#include "dbscqt_accountbooktreewidget.h"

#include <dbsc_account.h>
#include <dbsc_transaction.h>
#include <dbscqt_accountmodel.h>

#include <QPointer>

namespace dbscqt {
namespace {
  struct StoredAccountDisplayData
  {
    dbscqt::AccountItemData mAccountModelData;
    QPointer< dbscqt::AccountModel > mAccountModelHandle {};
  };
} // namespace
} // namespace dbscqt

auto dbscqt::createAccountItemData( dbsc::Account const& account ) -> dbscqt::AccountItemData
{
  return {
    .mName        = QString::fromStdString( account.name() ),
    .mDescription = QString::fromStdString( account.description() ),
    .mBalance     = QString::fromStdString( dbsc::TransactionUtil::currencyAsString( account.balance() ) ),
    .mId          = QUuid::fromString( account.id().view() ),
    .mIsOpen      = account.isOpen(),
  };
}
