// dbsqt_accountmodel.t.cpp
#include "dbsc_dbsserializer.h"

#include <dbsc_account.h>
#include <dbsc_accountbook.h>
#include <dbsc_dbstomlserializer.h>
#include <dbsc_transaction.h>
#include <dbsqt_accountmodel.h>

#include <QApplication>
#include <QHeaderView>
#include <QMainWindow>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <filesystem>
#include <ranges>

namespace {

std::filesystem::path const kAccountBookPath { std::filesystem::path( PROJECT_RESOURCES_DIR )
                                               / "testAccountBook.toml" };

static auto createAccountModelData( dbsc::Account const& account ) -> dbsqt::AccountModelData
{
  return {
    .mName        = QString::fromStdString( account.name() ),
    .mDescription = QString::fromStdString( account.description() ),
    .mBalance     = QString::fromStdString( dbsc::TransactionUtil::currencyAsString( account.balance() ) ),
    .mId          = QUuid::fromString( account.id().view() ),
    .mIsOpen      = account.isOpen(),
  };
}

static auto createTransactionItemData( dbsc::Transaction const& transaction, dbsc::AccountBook const& accountBook )
  -> dbsqt::TransactionItemData
{

  QDateTime const timestamp =
    QDateTime::fromStdTimePoint( std::chrono::time_point_cast< std::chrono::milliseconds >( transaction.timeStamp() ) );
  QString const transactionAmount =
    QString::fromStdString( dbsc::TransactionUtil::currencyAsString( transaction.amount() ) );
  QUuid const otherPartyId = QUuid::fromString( transaction.otherPartyId().view() );
  QString const otherPartyDisplayName =
    otherPartyId.isNull() ? "" : QString::fromStdString( accountBook.account( transaction.otherPartyId() ).name() );

  return {
    .mTimeStamp             = timestamp,
    .mTransactionAmount     = transactionAmount,
    .mNotes                 = QString::fromStdString( transaction.notes() ),
    .mOtherPartyAccountName = otherPartyDisplayName,
    .mTransactionId         = QUuid::fromString( transaction.transactionId().view() ),
    .mOtherPartyId          = otherPartyId,
  };
};

static auto createTransactionItems( dbsc::Account const& account, dbsc::AccountBook const& accountBook )
  -> std::vector< std::unique_ptr< dbsqt::TransactionItem > >
{
  auto transactionsSortedByDescendingDate =
    account | std::views::transform( []( auto const& transaction ) { return std::cref( transaction ); } )
    | std::ranges::to< std::vector >();
  std::ranges::sort( transactionsSortedByDescendingDate, std::greater<>(), []( auto&& item ) -> dbsc::TimeStamp {
    auto const& [_, transaction] = item.get();
    return transaction.timeStamp();
  } );

  std::vector< std::unique_ptr< dbsqt::TransactionItem > > items;
  items.reserve( account.transactionCount() );
  for ( auto const& item : transactionsSortedByDescendingDate ) {
    auto const& [id, transaction] = item.get();
    items.push_back(
      std::make_unique< dbsqt::TransactionItem >( createTransactionItemData( transaction, accountBook ) ) );
  }

  return items;
}
} // namespace

int main( int argc, char* argv[] )
{
  QApplication app { argc, argv };

  auto const kAccountBook = dbsc::loadAccountBook< dbsc::DbsTomlSerializer >( kAccountBookPath );
  auto const& kAccount    = ( *kAccountBook.cbegin() ).second;

  // NOLINTBEGIN(cppcoreguidelines-owning-memory)
  auto* mainWindow    = new QMainWindow();
  auto* centralWidget = new QWidget();
  {
    mainWindow->setCentralWidget( centralWidget );
    auto* widgetLayout = new QVBoxLayout( centralWidget );
    auto* tableView    = new QTableView();
    auto* tableModel   = new dbsqt::AccountModel(
      createAccountModelData( kAccount ), createTransactionItems( kAccount, kAccountBook ), nullptr );
    tableView->setModel( tableModel );
    tableView->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    widgetLayout->addWidget( tableView );
  }
  // NOLINTEND(cppcoreguidelines-owning-memory)
  mainWindow->showMaximized();
  return app.exec();
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
