// dbscqt_openaccountdialog.cpp
#include "dbscqt_openaccountdialog.h"

#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>
#include <ui_dbscqt_openaccountdialog.h>

dbscqt::OpenAccountDialog::OpenAccountDialog( QWidget* parent )
  : QDialog( parent )
  , mUi( std::make_shared< ::Ui::OpenAccountDialog >() )
{
  mUi->setupUi( this );
  QObject::connect( mUi->mAccountNameEdit, &QLineEdit::textChanged, [this]( QString const& text ) {
    auto* okButton = mUi->mButtonBox->button( QDialogButtonBox::Ok );
    okButton->setDisabled( text.isEmpty() );
  } );

  auto* okButton = mUi->mButtonBox->button( QDialogButtonBox::Ok );
  okButton->setDisabled( true );
}

auto dbscqt::OpenAccountDialog::accountName() const -> QString
{
  return mUi->mAccountNameEdit->text();
}

auto dbscqt::OpenAccountDialog::accountDescription() const -> QString
{
  return mUi->mDescriptionEdit->toPlainText();
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
