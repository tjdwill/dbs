// dbscqt_accountmodel.t.cpp
#include <dbsc_account.h>
#include <dbsc_accountbook.h>
#include <dbsc_tomlserializer.h>
#include <dbscqt_accountmodel.h>
#include <dbscqt_transactionitem.h>

#include <QApplication>
#include <QHeaderView>
#include <QMainWindow>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

#include <filesystem>

namespace {

std::filesystem::path const kAccountBookPath { std::filesystem::path( DBS_RESOURCES_DIR ) / "testAccountBook.toml" };

} // namespace

int main( int argc, char* argv[] )
{
  QApplication app { argc, argv };

  auto const kAccountBook = dbsc::readAccountBook< dbsc::TomlSerializer >( kAccountBookPath );
  auto const& kAccount    = ( *kAccountBook.cbegin() ).second;

  // NOLINTBEGIN(cppcoreguidelines-owning-memory)
  auto* mainWindow    = new QMainWindow();
  auto* centralWidget = new QWidget();
  {
    mainWindow->setCentralWidget( centralWidget );
    auto* widgetLayout = new QVBoxLayout( centralWidget );
    auto* tableView    = new QTableView();
    auto* tableModel   = new dbscqt::AccountModel( dbscqt::createTransactionItems( kAccount, kAccountBook ), nullptr );
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
