// dbscqt_preferenceswidget.cpp
#include "dbscqt_preferenceswidget.h"

#include <dbscqt_preferencepageinterface.h>

#include <bsls_assert.h>

#include <QtCore/QPointer>
#include <QtGui/QFont>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStackedWidget>

#include <map>
#include <set>

namespace dbscqt {
namespace {
  void setAllListItemsToUnmodifiedState( QListWidget* listWidget )
  {
    // Reset list item fonts to the "unmodified" state.
    for ( int listItemIndex = 0; listItemIndex < listWidget->count(); ++listItemIndex ) {
      auto* listItem              = listWidget->item( listItemIndex );
      auto fontForUnmodifiedItems = listItem->font();
      fontForUnmodifiedItems.setBold( false );
      listItem->setFont( fontForUnmodifiedItems );
    }
  }
} // namespace
} // namespace dbscqt

class dbscqt::PreferencesWidget::Private
{
public:
  std::set< QString > mSettingKeys;
  std::map< QString /* preferencePageDisplayName */, QPointer< dbscqt::PreferencePageInterface > > mPreferencePagesMap;
  QPointer< QSplitter > mWidgetSplitter;
  QPointer< QListWidget > mPreferencePageList;
  QPointer< QWidget > mPageDisplayContainer;
  QPointer< QDialogButtonBox > mDialogButtons;
  QPointer< QStackedWidget > mPreferencePagesStackedWidget;
};

dbscqt::PreferencesWidget::PreferencesWidget( QWidget* parent )
  : QDialog( parent )
{
}

dbscqt::PreferencesWidget::~PreferencesWidget() = default;

void dbscqt::PreferencesWidget::addPreferencePage( PreferencePageInterface* preferencePage )
{
  // Validation
  auto const preferenceCategory = preferencePage->preferenceDisplayName();
  if ( mImp->mPreferencePagesMap.contains( preferenceCategory ) ) {
    throw std::invalid_argument(
      QString( "A preference page that edits category `%1` already exists." ).arg( preferenceCategory ).toStdString() );
  }
  auto const& editedSettingsKeys = preferencePage->editedSettings();
  for ( auto const& key : editedSettingsKeys ) {
    if ( mImp->mSettingKeys.contains( key ) ) {
      throw std::invalid_argument(
        QString( "More than one page edits the following setting: %1" ).arg( key ).toStdString() );
    }
  }
  mImp->mSettingKeys.insert_range( editedSettingsKeys );

  // Take ownership of the page and add to the internal store
  mImp->mPreferencePageList->addItem( preferenceCategory );
  mImp->mPreferencePageList->sortItems();
  mImp->mPreferencePagesMap.insert( { preferenceCategory, preferencePage } );
  mImp->mPreferencePagesStackedWidget->addWidget( preferencePage );

  // Connect signals and slots
  QObject::connect( preferencePage,
                    &dbscqt::PreferencePageInterface::settingModified,
                    this,
                    &dbscqt::PreferencesWidget::handleSettingModified );
  QObject::connect( preferencePage,
                    &dbscqt::PreferencePageInterface::modifiedSettingsDiscarded,
                    this,
                    &dbscqt::PreferencesWidget::handlePreferencePageIsUpToDate );
  QObject::connect( preferencePage,
                    &dbscqt::PreferencePageInterface::settingsApplied,
                    this,
                    &dbscqt::PreferencesWidget::handlePreferencePageIsUpToDate );
}

void dbscqt::PreferencesWidget::applyAll()
{
  QSignalBlocker sb { this };
  for ( auto const& [category, page] : mImp->mPreferencePagesMap ) {
    page->apply();
  }

  // Reset list item fonts to the "unmodified" state.
  dbscqt::setAllListItemsToUnmodifiedState( mImp->mPreferencePageList );
}

void dbscqt::PreferencesWidget::discardChanges()
{
  QSignalBlocker sb { this };
  for ( auto const& [category, page] : mImp->mPreferencePagesMap ) {
    page->discardModifiedSettings();
  }

  // Reset list item fonts to the "unmodified" state.
  dbscqt::setAllListItemsToUnmodifiedState( mImp->mPreferencePageList );
}

void dbscqt::PreferencesWidget::handlePreferencePageIsUpToDate( QString const& preferencePageIdentifier )
{
  auto const items = mImp->mPreferencePageList->findItems( preferencePageIdentifier, Qt::MatchFlag::MatchExactly );
  BSLS_ASSERT( items.size() == 1 );
  auto* const updatedItem    = items.front();
  auto fontForUnmodifiedItem = updatedItem->font();
  fontForUnmodifiedItem.setBold( false );
  updatedItem->setFont( fontForUnmodifiedItem );
}

void dbscqt::PreferencesWidget::handleSettingModified( QString const& preferencePageIdentifier,
                                                       QString const& /* settingKey */,
                                                       QVariant const& /* proposedNewValue */ )
{
  auto const items = mImp->mPreferencePageList->findItems( preferencePageIdentifier, Qt::MatchFlag::MatchExactly );
  BSLS_ASSERT( items.size() == 1 );
  auto* const updatedItem  = items.front();
  auto fontForModifiedItem = updatedItem->font();
  fontForModifiedItem.setBold( true );
  updatedItem->setFont( fontForModifiedItem );
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
