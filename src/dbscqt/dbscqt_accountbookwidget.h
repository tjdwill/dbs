// dbscqt_accountbookwidget.h
#ifndef INCLUDED_DBSCQT_ACCOUNTBOOKWIDGET
#define INCLUDED_DBSCQT_ACCOUNTBOOKWIDGET

//@PURPOSE: Define a widget representing a dbsc::AccountBook
//
//@CLASSES:
//  dbscqt::AccountBookWidget: Qt representation of a dbsc::AccountBook.
//
//@DESCRIPTION: This component handles the user interaction needed to interface with the
//  DBS program.

#include <dbscqt_sharedapi.h>
#include <dbscqt_transactionitem.h>

#include <QtCore/QUuid>
#include <QtWidgets/QWidget>

#include <memory>

namespace dbsc {
class AccountBook;
} // namespace dbsc

namespace dbscqt {
class AccountItem;
class TransactionItem;

class DBSCQT_API AccountBookWidget : public QWidget
{
  Q_OBJECT
public:
  AccountBookWidget( std::shared_ptr< dbsc::AccountBook > const& accountBook, QWidget* parent = nullptr );
  ~AccountBookWidget() override;
  AccountBookWidget( AccountBookWidget const& )                        = delete;
  AccountBookWidget( AccountBookWidget&& ) noexcept                    = delete;
  auto operator=( AccountBookWidget const& ) -> AccountBookWidget&     = delete;
  auto operator=( AccountBookWidget&& ) noexcept -> AccountBookWidget& = delete;

Q_SIGNALS:
  void accountBookModified();
  void accountCreated( QUuid accountId );
  void accountStatusToggled( QUuid accountId, bool isNowActive );

  /// Send data necessary to update underlying Qt account model.
  void transactionComplete( QUuid accountId, TransactionItemData const& transactionItem );

public Q_SLOTS:
  void createAccount();

  /// Pop up a dialog window and collect information for a transaction.
  /// Writes the transaction to the underlying account book and oversees
  /// GUI updates.
  void createTransaction();

  void handleAccountBookSet( std::shared_ptr< dbsc::AccountBook > );
  void handleAccountSelected( AccountItem* );

  // Toggles the status of the account associated with the currently-focused
  // account item.
  void toggleAccountStatus();

private:
  void clearDisplay();

  /// Creates the relevant tree widget item and performs requisite layout and
  /// signal/slot configuration.
  void createAndInitializeAccountBookTreeWidget();

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
