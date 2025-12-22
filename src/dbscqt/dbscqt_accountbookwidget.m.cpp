// dbscqt_accountbookwidget.m.cpp
#include <dbsc_accountbook.h>
#include <dbsc_dbscserializer.h>
#include <dbsc_tomlserializer.h>
#include <dbscqt_accountbookwidget.h>

#include <QtCore/QScopedPointer>
#include <QtWidgets/QApplication>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

#include <filesystem>
#include <memory>

namespace {
std::filesystem::path const kAccountBookPath { std::filesystem::path( DBS_RESOURCES_DIR ) / "testAccountBook.toml" };
} // namespace

int main( int argc, char* argv[] )
{
  QApplication app { argc, argv };

  auto accountBook =
    std::make_shared< dbsc::AccountBook >( dbsc::readAccountBook< dbsc::TomlSerializer >( kAccountBookPath ) );

  auto mainWindow     = QScopedPointer( new QMainWindow() );
  auto* centralWidget = new QWidget();
  {
    mainWindow->setCentralWidget( centralWidget );
    auto* centralWidgetLayout = new QHBoxLayout( centralWidget );
    auto* accountBookWidget   = new dbscqt::AccountBookWidget( {} );
    centralWidgetLayout->addWidget( accountBookWidget );
    accountBookWidget->handleAccountBookSet( accountBook );
  }
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
