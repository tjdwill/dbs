// dbscqt_mainwindow.h
#ifndef INCLUDED_DBSCQT_MAINWINDOW
#define INCLUDED_DBSCQT_MAINWINDOW

///@PURPOSE: Define the application's main window.
///
///@CLASSES:
///  dbscqt::MainWindow: The application's main window. Responsible for coordinating
///     top-level actions.
///
///@DESCRIPTION: This component defines the application's main window, coordinating the
///  logic responsible for managing account books.

#include <dbscqt_sharedapi.h>

#include <QtWidgets/QMainWindow>

#include <filesystem>
#include <memory>

class QWidget;
class QCloseEvent;

namespace dbsc {
class AccountBook;
} // namespace dbsc

namespace dbscqt {
class PreferencesWidget;

class DBSCQT_API MainWindow final : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow( QWidget* parent = nullptr );
  ~MainWindow() final;
  MainWindow( MainWindow const& )                        = delete;
  MainWindow( MainWindow&& ) noexcept                    = delete;
  auto operator=( MainWindow const& ) -> MainWindow&     = delete;
  auto operator=( MainWindow&& ) noexcept -> MainWindow& = delete;

  void closeEvent( QCloseEvent* event ) final;

Q_SIGNALS:
  void accountBookLoaded( std::shared_ptr< dbsc::AccountBook > accountBookHandle );

public Q_SLOTS:
  /// Attempts to close the entire program, closing the current account book in
  /// the process.
  auto attemptExitProgram() -> bool;

  /// Closes the currently-loaded account book. If the account is in a modified
  /// state, this function prompts the user to save.
  /// @returns true if the account book was actually closed.
  auto closeAccountBook() -> bool;

  /// Create a new account book, prompting the user for a place to save it.
  void createNewAccountBook();

  /// Change the display and window state to communicate the account book's
  /// current state (saved/unsaved).
  void handleAccountBookModified( bool isModified );

  /// Loads the account book selected by the user. If no account book is
  /// selected or the account book cannot be found, this function is a no-op.
  void handleOpenAccountBookTriggered();

  void openPreferencesWindow();

  /// Write the current account book to file.
  ///
  /// @pre An account book is loaded. Does not have to be in a modified state.
  ///
  /// @return
  ///   - std::nullopt: The user canceled specifying a save path for an account
  ///       book that didn't have a previously-determined path (ex. an account
  ///       book that was recently created but has not been saved).
  ///   - true: save was successful.
  ///   - false: save was unsuccessful
  auto saveAccountBook() -> std::optional< bool >;

  /// Save the current account book to a user-specified location.
  /// On success, the new location is used for subsequent saving.
  auto saveAccountBookAs() -> std::optional< bool >;

  void showAboutPage();
  void showAboutQtPage();

private:
  static auto createPreferencesWidget( QWidget* parent ) -> dbscqt::PreferencesWidget*;
  /// Initiates account loading and handles the Ui updating associated with
  /// loading an account.
  void loadAccountBook( std::filesystem::path const& );

  /// @return Information regarding the user's interaction with the save prompt
  /// and the subsequent save operation.
  ///
  /// - std::nullopt: If the user never needs to be prompted in the first place.
  ///
  /// Otherwise, the values should be interpreted as follows:
  ///
  /// userProceededWithSaveOperation (first):
  ///   - std::nullopt: Canceled the save operation
  ///   - true: The user attempted to save the current account book.
  ///   - false: The user decided to proceed without saving
  /// saveSuccessfulResult (second):
  ///   - std::nullopt: The user canceled creating a file name for the account
  ///       book that didn't have a previously-determined path (ex. an account
  ///       book that was recently created but not saved).
  ///   - true: save was successful.
  ///   - false: save was unsuccessful
  auto promptUserToSaveIfAccountBookIsCurrentlyModified()
    -> std::optional< std::pair< std::optional< bool > /* userProceededWithSaveOperation */,
                                 std::optional< bool > /* successfulSaveOpt */ > >;

  /// @pre filePath is a valid path candidate.
  /// @return true if save was successful; false otherwise.
  auto saveAccountBookInternal( std::filesystem::path const& filePath ) -> bool;

  /// Determine if, based on prompting the user to save, the current operation
  /// calling this function should be aborted.
  auto shouldAbortOperation() -> bool;

  void updateAccountBookHandle( std::shared_ptr< dbsc::AccountBook > );

  /// @note if the account book has changed, ensure updateAccountBookHandle is
  /// called *before* calling this function.
  void updateWindowTitle( bool accountBookIsModified );

  class Private;
  std::unique_ptr< Private > mImp;
};
} // namespace dbscqt

#endif // header include guard

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
