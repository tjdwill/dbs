// dbs.m.cpp
// The main application of the project

#include <dbscqt_mainwindow.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/qnamespace.h>
#include <QtGui/QGuiApplication>
#include <QtGui/QStyleHints>
#include <QtWidgets/QApplication>

int main( int argc, char* argv[] )
{
  QCoreApplication::setApplicationName( "Digital Budgeting System" );
  QCoreApplication::setOrganizationName( "tjdwill" );             // ??
  QCoreApplication::setOrganizationDomain( "tjdwill.github.io" ); // ??

  QApplication dbsApp { argc, argv };
  dbscqt::MainWindow mainWindow {};

  QGuiApplication::styleHints()->setColorScheme( Qt::ColorScheme::Light );
  mainWindow.show();

  return dbsApp.exec();
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
