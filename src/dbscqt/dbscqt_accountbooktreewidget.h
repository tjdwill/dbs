// dbscqt_accountbooktreewidget.h
#ifndef INCLUDED_DBSCQT_ACCOUNTBOOKTREEWIDGET
#define INCLUDED_DBSCQT_ACCOUNTBOOKTREEWIDGET

//@PURPOSE: Define a hierarchical display for the dbsc::AccountBook
//
//@CLASSES:
//  dbscqt::AccountBookTreeWidget: A QTreeWidget subclass that represents an account book.
//  dbscqt::AccountItem: Represents an account within a tree-like hierarchy
//
//@DESCRIPTION: This component defines a tree widget for the account book for use in the
//  GUI display.
#include <dbscqt_transactionitem.h>

#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QUuid>

#include <memory>

namespace dbsc {
class AccountBook;
class Account;
} // namespace dbsc

namespace dbscqt {
class AccountBookTreeWidget;
class AccountModel;

struct AccountItemData
{

  QString mName;
  QString mDescription;
  QString mBalance;
  QUuid mId;
  bool mIsOpen;
};

[[nodiscard]] auto createAccountItemData( dbsc::Account const& account ) -> dbscqt::AccountItemData;

class AccountItem : public QTreeWidgetItem
{
public:
  AccountItem( dbsc::Account const& account, AccountBookTreeWidget* parent = nullptr );
  AccountItem( dbsc::Account const& account, QTreeWidgetItem* parent = nullptr );

  [[nodiscard]] auto accountModel( QUuid accountId ) const -> AccountModel*;
  [[nodiscard]] auto accountItemData( QUuid accountId ) const -> dbscqt::AccountItemData const&;

private:
  class Private;
  std::unique_ptr< Private > mImp;
};

class AccountBookTreeWidget : public QTreeWidget
{
  Q_OBJECT
public:
  AccountBookTreeWidget( std::shared_ptr< dbsc::AccountBook > const&, QWidget* parent = nullptr );

public Q_SLOTS:
  // void handleAccountCreated( dbscqt::AccountItemData );
  // void handleAccountStatusUpdated( QUuid accountId, bool isOpen );
  // /// Adds transaction data to the relevant model.
  // void handleTransactionMade( std::vector< TransactionItemData > transactionItems );
};

} // namespace dbscqt

#endif // include guard

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
