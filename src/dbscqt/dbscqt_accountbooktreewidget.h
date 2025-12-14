// dbscqt_accountbooktreewidget.h
#ifndef INCLUDED_DBSCQT_ACCOUNTBOOKTREEWIDGET
#define INCLUDED_DBSCQT_ACCOUNTBOOKTREEWIDGET

//@PURPOSE: Define a hierarchical display for the dbsc::AccountBook
//
//@CLASSES:
//  dbscqt::AccountBookTreeWidget: A QTreeWidget subclass that represents an account book.
//  dbscqt::AccountItem: Represents an account within a tree-like hierarchy
//  dbscqt::AccountItemData: PoD struct containing Qt-compatible, account-specific data.
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
  bool mIsActive { true };
};

[[nodiscard]] auto createAccountItemData( dbsc::Account const& account ) -> dbscqt::AccountItemData;

class AccountItem : public QTreeWidgetItem
{
public:
  // Since AccountItems are classified according to account active/inactive status, there will never be
  // a top-level AccountItem in an AccountBookTreeWidget. Therefore, the constructor with a
  // dbscqt::AccountBookTreeWidget parent item is omitted.

  ///@param accountModel takes ownership of the model
  AccountItem( dbscqt::AccountItemData accountData,
               dbscqt::AccountModel* accountModelHandle,
               QTreeWidgetItem* parent = nullptr );
  ~AccountItem() override;
  AccountItem( AccountItem const& )                        = delete;
  AccountItem( AccountItem&& ) noexcept                    = delete;
  auto operator=( AccountItem const& )                     = delete;
  auto operator=( AccountItem&& ) noexcept -> AccountItem& = delete;

  [[nodiscard]] auto accountModel() const -> AccountModel*;
  [[nodiscard]] auto accountItemData() const -> dbscqt::AccountItemData const&;
  [[nodiscard]] auto accountId() const -> QUuid;

  [[nodiscard]] auto accountItemDataMut() -> dbscqt::AccountItemData&;

private:
  class Private;
  std::unique_ptr< Private > mImp;
};

class AccountBookTreeWidget : public QTreeWidget
{
  Q_OBJECT
public:
  AccountBookTreeWidget( std::shared_ptr< dbsc::AccountBook > const&, QWidget* parent = nullptr );
  ~AccountBookTreeWidget() override;
  AccountBookTreeWidget( AccountBookTreeWidget const& )                        = delete;
  AccountBookTreeWidget( AccountBookTreeWidget&& ) noexcept                    = delete;
  auto operator=( AccountBookTreeWidget const& ) -> AccountBookTreeWidget&     = delete;
  auto operator=( AccountBookTreeWidget&& ) noexcept -> AccountBookTreeWidget& = delete;

  [[nodiscard]] auto currentAccountItem() const -> AccountItem*;
  [[nodiscard]] auto accountItemFromId( QUuid accountId ) const -> AccountItem*;
  [[nodiscard]] auto accountItemData( QUuid accountId ) const -> dbscqt::AccountItemData const&;
  [[nodiscard]] auto accountModel( QUuid accountId ) const -> AccountModel*;

  [[nodiscard]] auto accountItemDataMut( QUuid accountId ) -> dbscqt::AccountItemData&;
  [[nodiscard]] auto addAccountItem( AccountItem* ) -> bool;

Q_SIGNALS:
  void accountSelected( AccountItem* );
  /// Signal that some connected display should reset itself.
  void accountCleared();

public Q_SLOTS:
  /// @pre The new account has already been created and added to the current dbsc::AccountBook.
  void handleAccountCreated( QUuid accountId );
  void handleAccountStatusUpdated( QUuid accountId, bool isActive );

  /// Adds transaction data to the relevant model.
  void handleTransactionMade( QUuid accountId, TransactionItem* transactionData );

private:
  [[nodiscard]] auto isMemberOfThisTree( QTreeWidgetItem* ) -> bool;
  [[nodiscard]] auto isAccountItemInThisTree( QTreeWidgetItem* itemCandidate ) -> bool;
  [[nodiscard]] auto createAccountItem( dbsc::Account const& account, dbsc::AccountBook const& accountBook )
    -> AccountItem*;

  /// @return one of the top-level categorization items.
  [[nodiscard]] auto categoryItem( bool accountIsActive ) const -> QTreeWidgetItem*;

  class Private;
  std::unique_ptr< Private > mImp;
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
