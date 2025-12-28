// dbscqt_generalpreferenceswidget.h
#ifndef INCLUDED_DBSCQT_GENERALPREFERENCESWIDGET
#define INCLUDED_DBSCQT_GENERALPREFERENCESWIDGET

//@PURPOSE: Define a widget for general application preferences.
//
//@CLASSES:
//  dbscqt::GeneralPreferencesWidget: Edits the application's general preferences.
//
//@DESCRIPTION: This component defines a widget that edits general application settings.

#include <dbscqt_preferencekeys.h>
#include <dbscqt_preferencepageinterface.h>
#include <dbscqt_sharedapi.h>

#include <memory>

namespace dbscqt {
class DBSCQT_API GeneralPreferencesWidget final : public PreferencePageInterface
{
  Q_OBJECT
public:
  explicit GeneralPreferencesWidget( QWidget* parent = nullptr );
  ~GeneralPreferencesWidget() final;
  GeneralPreferencesWidget( GeneralPreferencesWidget const& )                        = delete;
  GeneralPreferencesWidget( GeneralPreferencesWidget&& ) noexcept                    = delete;
  auto operator=( GeneralPreferencesWidget const& ) -> GeneralPreferencesWidget&     = delete;
  auto operator=( GeneralPreferencesWidget&& ) noexcept -> GeneralPreferencesWidget& = delete;

  void apply() final;
  void discardModifiedSettings() final;
  auto editedSettings() const -> std::vector< QString > const& final;
  auto preferenceDisplayName() const -> QString const& final;

private:
  void handleColorSchemeSelectionChanged( QString const& colorSchemeName );

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
