// dbscqt_preferenceswidget.h
#ifndef INCLUDED_DBSCQT_PREFERENCESWIDGET
#define INCLUDED_DBSCQT_PREFERENCESWIDGET

//@PURPOSE: Provide an interactive window that enables users to modify program
//  preferences.
//
//@CLASSES:
//  - dbscqt::PreferencesWidget: A dialog window that has modifiable user options.
//  - dbscqt::PreferencePageInterface: A widget interface that is added
//    as a page to the PreferencesWidget.
//
//@DESCRIPTION: This component defines a widget that enables user preference
//  specification.

#include <dbscqt_sharedapi.h>

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtWidgets/QDialog>

#include <memory>

namespace dbscqt {

class PreferencePageInterface;

class DBSCQT_API PreferencesWidget : public QDialog
{
  Q_OBJECT
public:
  explicit PreferencesWidget( QWidget* parent = nullptr );
  ~PreferencesWidget() override;
  PreferencesWidget( PreferencesWidget const& )                        = delete;
  PreferencesWidget( PreferencesWidget&& ) noexcept                    = delete;
  auto operator=( PreferencesWidget const& ) -> PreferencesWidget&     = delete;
  auto operator=( PreferencesWidget&& ) noexcept -> PreferencesWidget& = delete;

  /// Adds a preference page.
  /// @throws @c std::invalid_argument if there is a duplicate setting or
  /// category name among the other registered pages.
  void addPreferencePage( PreferencePageInterface* );

public Q_SLOTS:
  /// Apply the settings that have been modified.
  void applyAll();

  /// Discard modified settings.
  void discardChanges();

  /// Update the gui display name for the current preference page.
  void handlePreferencePageIsUpToDate( QString const& preferencePageIdentifier );

  /// Update the GUI to visually display that a category has unsaved modifications.
  void handleSettingModified( QString const& preferencePageIdentifier,
                              QString const& /* settingKey */,
                              QVariant const& /* proposedNewValue */ );

private:
  class Private;
  std::unique_ptr< Private > mImp;
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
