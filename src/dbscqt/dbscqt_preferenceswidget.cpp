// dbscqt_preferenceswidget.cpp
#include "dbscqt_preferenceswidget.h"

#include <dbscqt_preferencepageinterface.h>

#include <bsls_assert.h>

#include <QtCore/QPointer>
#include <QtCore/QSignalBlocker>
#include <QtGui/QFont>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
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
  QPointer< QDialogButtonBox > mDialogButtons;
  QPointer< QStackedWidget > mPreferencePagesStackedWidget;
  QPointer< QScrollArea > mDisplayScrollArea;
};

dbscqt::PreferencesWidget::PreferencesWidget( QWidget* parent )
  : QDialog( parent )
{
  using ButtonType = QDialogButtonBox::StandardButton;
  auto mainLayout  = QPointer( new QVBoxLayout( this ) );
  {
    auto const dialogButtons = ButtonType::Ok | ButtonType::Cancel | ButtonType::Reset | ButtonType::Apply;
    mImp->mDialogButtons     = QPointer( new QDialogButtonBox( dialogButtons, Qt::Orientation::Horizontal, this ) );
    {
      mImp->mDialogButtons->setObjectName( "mDialogButtons" );
      QObject::connect( mImp->mDialogButtons->button( ButtonType::Apply ), &QPushButton::clicked, [this]() {
        auto* currentItem = mImp->mPreferencePageList->currentItem();
        BSLS_ASSERT( currentItem );
        auto preferencePage = mImp->mPreferencePagesMap.at( currentItem->text() );
        preferencePage->apply();
      } );
      QObject::connect( mImp->mDialogButtons->button( ButtonType::Reset ), &QPushButton::clicked, [this]() {
        auto* currentItem = mImp->mPreferencePageList->currentItem();
        BSLS_ASSERT( currentItem );
        auto preferencePage = mImp->mPreferencePagesMap.at( currentItem->text() );
        preferencePage->discardModifiedSettings();
      } );
      QObject::connect( mImp->mDialogButtons->button( ButtonType::Ok ),
                        &QPushButton::clicked,
                        this,
                        &dbscqt::PreferencesWidget::applyAll );
      QObject::connect( mImp->mDialogButtons->button( ButtonType::Cancel ),
                        &QPushButton::clicked,
                        this,
                        &dbscqt::PreferencesWidget::discardChanges );
    }
    mImp->mDisplayScrollArea = QPointer( new QScrollArea() );
    {
      mImp->mDisplayScrollArea->setObjectName( "mDisplayScrollArea" );
      mImp->mDisplayScrollArea->setWidgetResizable( true );
      mImp->mDisplayScrollArea->setFrameShape( QFrame::NoFrame );

      mImp->mPreferencePagesStackedWidget = QPointer( new QStackedWidget() );
      mImp->mDisplayScrollArea->setWidget( mImp->mPreferencePagesStackedWidget );
    }

    mImp->mPreferencePageList = QPointer( new QListWidget() );
    mImp->mWidgetSplitter     = QPointer( new QSplitter( Qt::Orientation::Horizontal ) );
    mImp->mWidgetSplitter->addWidget( mImp->mPreferencePageList );
    mImp->mWidgetSplitter->addWidget( mImp->mDisplayScrollArea );
    mImp->mWidgetSplitter->setStretchFactor( mImp->mWidgetSplitter->indexOf( mImp->mDisplayScrollArea ), 1 );

    mainLayout->addWidget( mImp->mWidgetSplitter );
    mainLayout->addWidget( mImp->mDialogButtons );
  }

  QObject::connect( mImp->mPreferencePageList,
                    &QListWidget::currentItemChanged,
                    [this]( QListWidgetItem* current, QListWidgetItem* /* previous */ ) {
                      // Update apply and reset button visibility/state.
                      // Update currently-displayed preference page.
                      bool const isValidItem = static_cast< bool >( current );
                      updateApplyAndResetButtonVisibility( isValidItem );

                      if ( isValidItem ) {
                        auto const preferencePage = mImp->mPreferencePagesMap[current->text()];
                        updateApplyAndResetButtonsEnabled( preferencePage->hasModifiedSettings() );

                        mImp->mPreferencePagesStackedWidget->setCurrentWidget( preferencePage );
                      } else {
                        mImp->mPreferencePagesStackedWidget->setCurrentIndex( -1 );
                      }
                    } );
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

  dbscqt::setAllListItemsToUnmodifiedState( mImp->mPreferencePageList );
  updateApplyAndResetButtonsEnabled( false );
}

void dbscqt::PreferencesWidget::discardChanges()
{
  QSignalBlocker sb { this };
  for ( auto const& [category, page] : mImp->mPreferencePagesMap ) {
    page->discardModifiedSettings();
  }

  dbscqt::setAllListItemsToUnmodifiedState( mImp->mPreferencePageList );
  updateApplyAndResetButtonsEnabled( false );
}

void dbscqt::PreferencesWidget::handlePreferencePageIsUpToDate( QString const& preferencePageIdentifier )
{
  auto const items = mImp->mPreferencePageList->findItems( preferencePageIdentifier, Qt::MatchFlag::MatchExactly );
  BSLS_ASSERT( items.size() == 1 );
  auto* const updatedItem    = items.front();
  auto fontForUnmodifiedItem = updatedItem->font();
  fontForUnmodifiedItem.setBold( false );
  updatedItem->setFont( fontForUnmodifiedItem );
  updateApplyAndResetButtonsEnabled( false );
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
  updateApplyAndResetButtonsEnabled( true );
}

void dbscqt::PreferencesWidget::updateApplyAndResetButtonVisibility( bool const isVisible )
{

  auto* applyButton = mImp->mDialogButtons->button( QDialogButtonBox::StandardButton::Apply );
  auto* resetButton = mImp->mDialogButtons->button( QDialogButtonBox::StandardButton::Reset );

  applyButton->setVisible( isVisible );
  resetButton->setVisible( isVisible );
}

void dbscqt::PreferencesWidget::updateApplyAndResetButtonsEnabled( bool const isEnabled )
{
  auto* applyButton = mImp->mDialogButtons->button( QDialogButtonBox::StandardButton::Apply );
  auto* resetButton = mImp->mDialogButtons->button( QDialogButtonBox::StandardButton::Reset );

  applyButton->setEnabled( isEnabled );
  resetButton->setEnabled( isEnabled );
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
