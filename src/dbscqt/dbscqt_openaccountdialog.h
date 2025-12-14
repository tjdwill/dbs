// dbscqt_openaccountdialog.h
#ifndef INCLUDED_DBSCQT_OPENACCOUNTDIALOG
#define INCLUDED_DBSCQT_OPENACCOUNTDIALOG

//@PURPOSE: Define a widget used to collect information for opening an account.
//
//@CLASSES:
//  dbscqt::OpenAccountDialog: A simple dialog that collects information.
//
//@DESCRIPTION: This component defines a dialog for opening an account in DBS.

#include <QtCore/QString>
#include <QtWidgets/QDialog>

#include <memory>

namespace Ui { // NOLINT
class OpenAccountDialog;
} // namespace Ui

namespace dbscqt {
class OpenAccountDialog : public QDialog
{
  Q_OBJECT
public:
  explicit OpenAccountDialog( QWidget* parent = nullptr );

  // These methods should only be called if the dialog was accepted.
  [[nodiscard]] auto accountName() const -> QString;
  [[nodiscard]] auto accountDescription() const -> QString;

private:
  std::shared_ptr< ::Ui::OpenAccountDialog > mUi;
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
