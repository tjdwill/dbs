// dbscqt_preferencepageinterface.cpp
#include "dbscqt_preferencepageinterface.h"

#include <QtCore/QSettings>

dbscqt::PreferencePageInterface::PreferencePageInterface( QWidget* parent )
  : QWidget( parent )
{
  QObject::connect(
    this, &PreferencePageInterface::settingModified, this, &PreferencePageInterface::handleSettingModified );
}

void dbscqt::PreferencePageInterface::apply()
{
  for ( auto const& [settingKey, newValue] : mModifiedSettings ) {
    QSettings().setValue( settingKey, newValue );
  }
  Q_EMIT settingsApplied( preferenceDisplayName() );
}

void dbscqt::PreferencePageInterface::discardModifiedSettings()
{
  mModifiedSettings.clear();
  Q_EMIT modifiedSettingsDiscarded( preferenceDisplayName() );
}

void dbscqt::PreferencePageInterface::handleSettingModified( QString const& /* preferencePageIdentifier */,
                                                             QString const& settingKey,
                                                             QVariant const& proposedNewValue )
{
  // Inserts the key-value pair if it does not exist already in the store.
  // Overwrites the value if already present.
  mModifiedSettings[settingKey] = proposedNewValue;
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
