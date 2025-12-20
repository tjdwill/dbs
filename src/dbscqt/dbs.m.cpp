// dbs.m.cpp
// The main application of the project

#include <dbscqt_mainwindow.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/qnamespace.h>
#include <QtWidgets/QApplication>

int main( int argc, char* argv[] )
{
  QCoreApplication::setApplicationName( "Digital Budgeting System" );
  QCoreApplication::setOrganizationName( "tjdwill" );             // ??
  QCoreApplication::setOrganizationDomain( "tjdwill.github.io" ); // ??

  QApplication dbsApp { argc, argv };
  dbscqt::MainWindow mainWindow {};

  mainWindow.show();

  return dbsApp.exec();
}
