// dbscqt_displayutil.cpp
#include "dbscqt_displayutil.h"

#include <dbsc_account.h>
#include <dbsc_accountbook.h>
#include <dbsc_transaction.h>

auto dbscqt::DisplayUtil::createAccountModelData( dbsc::Account const& account ) -> dbscqt::AccountModelData
{
  return {
    .mName        = QString::fromStdString( account.name() ),
    .mDescription = QString::fromStdString( account.description() ),
    .mBalance     = QString::fromStdString( dbsc::TransactionUtil::currencyAsString( account.balance() ) ),
    .mId          = QUuid::fromString( account.id().view() ),
    .mIsOpen      = account.isOpen(),
  };
}

auto dbscqt::DisplayUtil::createTransactionItemData( dbsc::Transaction const& transaction,
                                                     dbsc::AccountBook const& accountBook )
  -> dbscqt::TransactionItemData
{

  QDateTime const timestamp =
    QDateTime::fromStdTimePoint( std::chrono::time_point_cast< std::chrono::milliseconds >( transaction.timeStamp() ) );
  QString const transactionAmount =
    QString::fromStdString( dbsc::TransactionUtil::currencyAsString( transaction.amount() ) );
  QUuid const otherPartyId = QUuid::fromString( transaction.otherPartyId().view() );
  QString const otherPartyDisplayName =
    otherPartyId.isNull() ? "" : QString::fromStdString( accountBook.account( transaction.otherPartyId() ).name() );

  return {
    .mTimeStamp             = timestamp,
    .mTransactionAmount     = transactionAmount,
    .mNotes                 = QString::fromStdString( transaction.notes() ),
    .mOtherPartyAccountName = otherPartyDisplayName,
    .mTransactionId         = QUuid::fromString( transaction.transactionId().view() ),
    .mOtherPartyId          = otherPartyId,
  };
};

auto dbscqt::DisplayUtil::accountNameWithShortenedUuid( QUuid id, QString const& name ) -> QString
{
  return QString( "%1 (%2)" ).arg( name ).arg( id.toString( QUuid::WithoutBraces ).split( '-' ).front() );
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
