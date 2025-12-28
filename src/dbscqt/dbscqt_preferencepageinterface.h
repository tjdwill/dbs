// dbscqt_PreferencePageInterface
#ifndef INCLUDED_DBSCQT_PREFERENCEPAGEINTERFACE
#define INCLUDED_DBSCQT_PREFERENCEPAGEINTERFACE

//@PURPOSE: Define an interface for defining a widget that edits some set of
//  user preferences.
//
//@CLASSES:
//  dbscqt::PreferencePageInterface: An abstract widget that edits some
//    set of user preferences. A given preference settings key is assumed to be
//    unique.
//
//@DESCRIPTION: This component defines an interface for editing application
//  preferences.

#include <dbscqt_sharedapi.h>

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtWidgets/QWidget>

#include <map>

namespace dbscqt {
class DBSCQT_API PreferencePageInterface : public QWidget
{
  Q_OBJECT
public:
  explicit PreferencePageInterface( QWidget* parent = nullptr );

  /// Apply the modified settings, writing the new values to QSettings.
  void apply();

  /// Revert modified settings to their original state. Subclasses must
  /// implement GUI-specific logic, resetting the editors to the modified
  /// settings' original values. Subclasses must call this interface's
  /// implementation at the end of their override implementations to update the
  /// internal tracking of modified settings and signal the setting
  /// modifications were discarded.
  virtual void discardModifiedSettings();

  /// @return a collection of QSettings keys corresponding to the settings this
  /// widget edits.
  virtual auto editedSettings() const -> std::vector< QString > const& = 0;

  /// @return true if there are unapplied, modified setting values.
  auto hasModifiedSettings() const -> bool;

  /// @return the name of the "category" of preferences this page edits.
  /// @note Assumed to be unique to this page.
  virtual auto preferenceDisplayName() const -> QString const& = 0;

Q_SIGNALS:
  /// Sends an identifier of the preference page (i.e. the display name) whose
  /// changes have been discarded.
  /// @note Subclasses *do not* need to emit this in implementations of `discardModifiedSettings`.
  ///   The abstract interface emits this signal.
  void modifiedSettingsDiscarded( QString const& preferencePageIdentifier );

  void settingsApplied( QString const& preferencePageIdentifier );

  /// Signal that the setting referred to by @p settingKey has been modified in
  /// the GUI. Modifying a setting does *not* automatically update the setting
  /// with the new value.
  void settingModified( QString const& preferencePageIdentifier,
                        QString const& settingKey,
                        QVariant const& proposedNewValue );

private Q_SLOTS:
  /// Updates the internal store of the modified settings.
  void handleSettingModified( QString const& preferencePageIdentifier,
                              QString const& settingKey,
                              QVariant const& proposedNewValue );

private:
  std::map< QString /* settingKey */, QVariant /* proposedNewValue */ > mModifiedSettings;
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
