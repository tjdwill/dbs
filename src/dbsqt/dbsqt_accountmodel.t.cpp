// dbsqt_accountmodel.t.cpp
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

#include <filesystem>

namespace {
std::filesystem::path const kAccountBookPath { std::filesystem::path( PROJECT_RESOURCES_DIR )
                                               / "testAccountBook.toml" };

static auto createTransactionItems( dbsc::Account const& account )
  -> std::vector< std::unique_ptr< dbsqt::TransactionItem > >
{
  std::vector< std::unique_ptr< dbsqt::TransactionItem > > items;
  items.reserve( account.transactionCount() );
  for ( auto const& [id, transaction] : account ) {
    items.push_back( std::make_unique< dbsqt::TransactionItem >(
      QUuid::fromString( id.view() ),
      QUuid::fromString( transaction.otherPartyId().view() ),
      QString::fromStdString( dbsc::TransactionUtil::currencyAsString( transaction.amount() ) ),
      QDateTime::fromStdTimePoint(
        std::chrono::time_point_cast< std::chrono::milliseconds >( transaction.timeStamp() ) ),
      QString::fromStdString( transaction.notes() ) ) );
  }
  return items;
}
} // namespace

int main( int argc, char* argv[] )
{
  QApplication app { argc, argv };

  auto const kAccountBook = dbsc::DbsTomlSerializer::readAccountBook( kAccountBookPath );
  auto const& kAccount    = ( *kAccountBook.cbegin() ).second;

  auto* mainWindow    = new QMainWindow();
  auto* centralWidget = new QWidget();
  {
    mainWindow->setCentralWidget( centralWidget );
    auto* widgetLayout = new QVBoxLayout( centralWidget );
    auto* tableView    = new QTableView();
    auto* tableModel =
      new dbsqt::AccountModel( createTransactionItems( kAccount ),
                               QUuid::fromString( kAccount.id().view() ),
                               QString::fromStdString( kAccount.name() ),
                               QString::fromStdString( kAccount.description() ),
                               QString::fromStdString( dbsc::TransactionUtil::currencyAsString( kAccount.balance() ) ),
                               kAccount.isOpen(),
                               nullptr );
    tableView->setModel( tableModel );
    tableView->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    widgetLayout->addWidget( tableView );
  }

  mainWindow->show();
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
