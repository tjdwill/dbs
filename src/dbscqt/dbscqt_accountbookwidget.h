// dbscqt_accountbookwidget.h
#ifndef INCLUDED_DBSCQT_ACCOUNTBOOKWIDGET
#define INCLUDED_DBSCQT_ACCOUNTBOOKWIDGET
//@PURPOSE: Define a widget representing a dbsc::AccountBook
//
//@CLASSES:
//  dbscqt::AccountBookWidget: Qt representation of a dbsc::AccountBook.
//
//@DESCRIPTION:

#include <dbsc_accountbook.h>
#include <dbscqt_accountmodel.h>
#include <dbscqt_displayutil.h>

#include <QWidget>

#include <memory>

namespace dbscqt {
class AccountBookWidget : public QWidget
{
  Q_OBJECT
public:
  AccountBookWidget( std::shared_ptr< dbsc::AccountBook > const& accountBook, QWidget* parent = nullptr );
  ~AccountBookWidget() override;
  AccountBookWidget( AccountBookWidget const& )                        = delete;
  AccountBookWidget( AccountBookWidget&& ) noexcept                    = delete;
  auto operator=( AccountBookWidget const& ) -> AccountBookWidget&     = delete;
  auto operator=( AccountBookWidget&& ) noexcept -> AccountBookWidget& = delete;

public Q_SLOTS:
  void handleAccountBookSet( std::shared_ptr< dbsc::AccountBook > );
  void handleAccountSelected( QUuid accountId );
  // void handleAccountCreated( QUuid accountId );
  // void handleAccountStatusChanged( QUuid accountId, bool isOpened );
  // /// Adds transaction data to the relevant model.
  // void handleTransactionMade( std::vector< TransactionItemData > transactionItems );

private:
  void clearDisplay();
  void deleteAccountModels();
  void refreshAccountComboBox();

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
