// dbscqt_transactiondialog.m.cpp
#include <dbscqt_displayutil.h>
#include <dbscqt_transactiondialog.h>

#include <QtCore/QScopedPointer>
#include <QtCore/QString>
#include <QtCore/QUuid>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>

#include <utility>
#include <vector>

int main( int argc, char* argv[] )
{
  QApplication app { argc, argv };

  QUuid const externalId;
  QString const externalDisplayName { "External" };
  QUuid const primaryPartyId = QUuid::createUuid();
  QString const primaryDisplayName { dbscqt::DisplayUtil::accountNameWithShortenedUuid( primaryPartyId,
                                                                                        "tjdwill's Account" ) };

  std::vector< std::pair< QUuid, QString > > activeAccounts {
    { primaryPartyId, primaryDisplayName }
  };
  constexpr int kGeneratedActiveAccountCount = 5;
  for ( int i = kGeneratedActiveAccountCount; i > 0; --i ) { // count down to test combo box sorting logic
    auto const id          = QUuid::createUuid();
    auto const accountName = QString( "Account%1" ).arg( i );
    activeAccounts.push_back( { id, dbscqt::DisplayUtil::accountNameWithShortenedUuid( id, accountName ) } );
  }

  QScopedPointer mainWidget { new dbscqt::TransactionDialog( { .mExternalAccountId          = externalId,
                                                               .mExternalAccountDisplayName = externalDisplayName,
                                                               .mPrimaryPartyId             = primaryPartyId,
                                                               .mPrimaryPartyDisplayName    = primaryDisplayName,
                                                               .mActiveAccountData = std::move( activeAccounts ) } ) };
  mainWidget->setModal( true );
  mainWidget->show();
  QObject::connect( mainWidget.get(), &QDialog::accepted, [&mainWidget]() {
    auto const message = QString( "Transaction made between %1 and %2 to the tune of $%3.\n\nNotes: %4" )
                           .arg( mainWidget->primaryPartyId().toString() )
                           .arg( mainWidget->otherPartyId().toString() )
                           .arg( mainWidget->amount() )
                           .arg( mainWidget->notes() );
    QMessageBox::information( nullptr, "Transaction Recorded", message );
  } );

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
