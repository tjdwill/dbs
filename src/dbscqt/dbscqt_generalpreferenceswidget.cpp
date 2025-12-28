// dbscqt_generalpreferenceswidget.cpp
#include "dbscqt_generalpreferenceswidget.h"

#include <dbsutl_helpers.h>

#include <Qt>
#include <QtCore/QPointer>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QGuiApplication>
#include <QtGui/QStyleHints>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>

#include <map>
#include <vector>

namespace dbscqt {

namespace {
  auto colorSchemeToStringMap() -> std::map< Qt::ColorScheme, QString > const&
  {
    static std::map< Qt::ColorScheme, QString > const kColorSchemeToStringMap {
      {    Qt::ColorScheme::Dark,    "Dark" },
      {   Qt::ColorScheme::Light,   "Light" },
      { Qt::ColorScheme::Unknown, "Default" },
    };

    return kColorSchemeToStringMap;
  }

  auto colorSchemeToString( Qt::ColorScheme colorScheme ) -> QString const&
  {
    return colorSchemeToStringMap().at( colorScheme );
  }

  auto colorSchemeFromString( QString const& colorSchemeName ) -> Qt::ColorScheme
  {
    static std::map< QString, Qt::ColorScheme > const kColorSchemeFromStringMap =
      dbsutl::createReversedMap( colorSchemeToStringMap() );

    return kColorSchemeFromStringMap.at( colorSchemeName );
  }
} // namespace
} // namespace dbscqt

class dbscqt::GeneralPreferencesWidget::Private
{
public:
  QPointer< QComboBox > mColorSchemeSelectionBox;
};

dbscqt::GeneralPreferencesWidget::GeneralPreferencesWidget( QWidget* parent )
  : dbscqt::PreferencePageInterface( parent )
  , mImp( std::make_unique< Private >() )
{
  setObjectName( "dbscqt::GeneralPreferencesWidget" );
  auto mainLayout = QPointer( new QFormLayout( this ) );
  // Color Scheme Editor
  {

    mImp->mColorSchemeSelectionBox = QPointer( new QComboBox() );
    mImp->mColorSchemeSelectionBox->addItems( {
      dbscqt::colorSchemeToString( Qt::ColorScheme::Light ),
      dbscqt::colorSchemeToString( Qt::ColorScheme::Dark ),
      dbscqt::colorSchemeToString( Qt::ColorScheme::Unknown ),
    } );
    // Set initial combo box state.
    mImp->mColorSchemeSelectionBox->setCurrentText(
      QSettings().value( dbscqt::PreferenceKeys::kApplicationColorSchemeKey ).toString() );

    QObject::connect( mImp->mColorSchemeSelectionBox,
                      &QComboBox::currentTextChanged,
                      this,
                      &dbscqt::GeneralPreferencesWidget::handleColorSchemeSelectionChanged );

    mainLayout->addRow( "Color Scheme:", mImp->mColorSchemeSelectionBox );
  }
}

dbscqt::GeneralPreferencesWidget::~GeneralPreferencesWidget() = default;

void dbscqt::GeneralPreferencesWidget::apply()
{
  // Update QSettings
  dbscqt::PreferencePageInterface::apply();

  // Update GUI
  auto const colorSchemeSettingValue = QSettings()
                                         .value( dbscqt::PreferenceKeys::kApplicationColorSchemeKey,
                                                 dbscqt::colorSchemeToString( Qt::ColorScheme::Unknown ) )
                                         .toString();
  auto const colorScheme = colorSchemeFromString( colorSchemeSettingValue );
  QGuiApplication::styleHints()->setColorScheme( colorScheme );

  Q_EMIT settingsApplied( preferenceDisplayName() );
}

void dbscqt::GeneralPreferencesWidget::discardModifiedSettings()
{
  dbscqt::PreferencePageInterface::discardModifiedSettings();

  mImp->mColorSchemeSelectionBox->setCurrentText(
    QSettings().value( dbscqt::PreferenceKeys::kApplicationColorSchemeKey ).toString() );

  Q_EMIT modifiedSettingsDiscarded( preferenceDisplayName() );
}

auto dbscqt::GeneralPreferencesWidget::editedSettings() const -> std::vector< QString > const&
{
  static std::vector< QString > const kEditedSettings {
    dbscqt::PreferenceKeys::kApplicationColorSchemeKey,
  };

  return kEditedSettings;
}

auto dbscqt::GeneralPreferencesWidget::preferenceDisplayName() const -> QString const&
{
  static QString const kDisplayName = "General";

  return kDisplayName;
}

void dbscqt::GeneralPreferencesWidget::handleColorSchemeSelectionChanged( QString const& colorSchemeName )
{
  Q_EMIT settingModified(
    preferenceDisplayName(), dbscqt::PreferenceKeys::kApplicationColorSchemeKey, colorSchemeName );
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
