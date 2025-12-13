// dbscqt_transactiondialog.cpp
#include "dbscqt_transactiondialog.h"

#include <QtGui/QDoubleValidator>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>

#include <algorithm>
#include <functional>

dbscqt::TransactionDialog::TransactionDialog( dbscqt::TransactionDialog::ConstructorArgs args, QWidget* parent )
  : QDialog( parent )
  , mPrimaryPartyId( args.mPrimaryPartyId )
{
  mUi.setupUi( this );
  mUi.mPrimaryPartyDisplay->setText( args.mPrimaryPartyDisplayName );
  mUi.mOtherPartySelectionBox->addItem( args.mExternalAccountDisplayName, args.mExternalAccountId );

  std::ranges::sort( args.mActiveAccountData, std::less(), []( auto const& idDisplayNamePair ) -> QString const& {
    return idDisplayNamePair.second;
  } );
  for ( auto const& [activeAccountId, displayName] : args.mActiveAccountData ) {
    if ( activeAccountId != mPrimaryPartyId ) {
      mUi.mOtherPartySelectionBox->addItem( displayName, activeAccountId );
    }
  }

  auto* currencyValidator = new QDoubleValidator( mUi.mTransactionAmountEdit );
  currencyValidator->setDecimals( 2 );
  currencyValidator->setNotation( QDoubleValidator::StandardNotation );
  mUi.mTransactionAmountEdit->setValidator( currencyValidator );

  QObject::connect( mUi.mTransactionAmountEdit, &QLineEdit::textEdited, [this]() {
    auto* okButton               = mUi.mButtonBox->button( QDialogButtonBox::Ok );
    bool const okButtonIsEnabled = okButton->isEnabled();
    okButton->setEnabled( okButtonIsEnabled && mUi.mTransactionAmountEdit->hasAcceptableInput() );
  } );

  // Set initial widget state.
  mUi.mButtonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
}

void dbscqt::TransactionDialog::handleAccepted()
{
  mOtherPartyId      = mUi.mOtherPartySelectionBox->currentData().toUuid();
  mTransactionAmount = mUi.mTransactionAmountEdit->text();
  mTransactionNotes  = mUi.mNotesEdit->toPlainText();
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
