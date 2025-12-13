// dbscqt_transactiondialog.h
#ifndef INCLUDED_DBSCQT_TRANSACTIONDIALOG
#define INCLUDED_DBSCQT_TRANSACTIONDIALOG

//@PURPOSE: Define a dialog used to make a transaction
//
//@CLASSES: dbscqt::TransactionDialog: A dialog that assists in collecting user
//  input for a transaction.
//
//@DESCRIPTION: This component defines a dialog widget to collect transaction
//  information. While this is its own component, its interface is designed to
//  work with the data stored in dbscqt::AccountBookTreeWidget and
//  dbscqt::AccountBookWidget.

#include <QtCore/QString>
#include <QtCore/QUuid>
#include <QtWidgets/QDialog>

#include <memory>
#include <utility>
#include <vector>

namespace Ui // NOLINT
{
class TransactionDialog;
} // namespace Ui

namespace dbscqt {

/// This dialog's getter methods only return valid values if the dialog was
/// accepted. Connect to this widget's `accepted` signal
class TransactionDialog : public QDialog
{
  Q_OBJECT
public:
  struct ConstructorArgs
  {
    // Information needed to communicate that a transaction was with a
    // DBS-external entity.
    QUuid mExternalAccountId {};
    QString mExternalAccountDisplayName {};

    // Information about the active accounts eligible for a transaction.
    QUuid mPrimaryPartyId {};
    QString mPrimaryPartyDisplayName {};
    std::vector< std::pair< QUuid /*activeAccountId*/, QString /*displayName*/ > > mActiveAccountData {};
  };

  explicit TransactionDialog( ConstructorArgs args, QWidget* parent = nullptr );

  /// @return the amount of money exchanged in the transaction *from the primary
  ///   party's perspective*.
  /// @note Currently assumes the American dollar as the currency.
  [[nodiscard]] auto amount() const -> QString const&;

  [[nodiscard]] auto primaryPartyId() const -> QUuid;
  [[nodiscard]] auto otherPartyId() const -> QUuid;
  [[nodiscard]] auto notes() const -> QString const&;

public Q_SLOTS:
  void handleAccepted();

private:
  std::shared_ptr< ::Ui::TransactionDialog > mUi {};
  QUuid mPrimaryPartyId;
  QUuid mOtherPartyId;
  QString mTransactionAmount;
  QString mTransactionNotes;
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
