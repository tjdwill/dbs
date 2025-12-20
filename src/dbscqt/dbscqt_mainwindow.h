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

#include <QtWidgets/QMainWindow>

#include <filesystem>
#include <memory>

class QWidget;

namespace dbsc {
class AccountBook;
} // namespace dbsc

namespace dbscqt {
class MainWindow final : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow( QWidget* parent = nullptr );
  ~MainWindow() final;
  MainWindow( MainWindow const& )                        = delete;
  MainWindow( MainWindow&& ) noexcept                    = delete;
  auto operator=( MainWindow const& ) -> MainWindow&     = delete;
  auto operator=( MainWindow&& ) noexcept -> MainWindow& = delete;

Q_SIGNALS:
  void accountBookLoaded( std::shared_ptr< dbsc::AccountBook > accountBookHandle );
  void shutdownInitiated();

public Q_SLOTS:
  /// Closes the currently-loaded account book. If the account is in a modified
  /// state, this function prompts the user to save.
  /// @returns true if the account book was actually closed.
  auto closeAccountBook() -> bool;

  /// Create a new account book, prompting the user for a place to save it.
  void createNewAccountBook();

  /// Closes the entire program, closing the current account book in the process.
  void exitProgram();

  /// Change the display and window state to communicate the account book is in an unsaved
  /// state.
  void handleAccountBookModified( bool isModified );

  /// Loads the account book selected by the user. If no account book is selected, or the
  /// account book cannot be found, this function displays the welcome screen (after a
  /// popup message in the latter case).
  void handleOpenAccountBookTriggered();

  /// Write the current account book to file.
  /// @pre An account book is loaded. Does not have to be in a modified state.
  /// @return
  ///   - std::nullopt: The user canceled creating a file name for the account book that didn't have a
  ///       previously-determined path (ex. an account book that was recently created but not saved).
  ///   - true: save was successful.
  ///   - false: save was unsuccessful
  auto saveAccountBook() -> std::optional< bool >;

  void showAboutPage();
  void showAboutQtPage();

private:
  auto loadAccountBookInternal( std::filesystem::path const& ) -> std::shared_ptr< dbsc::AccountBook >;

  /// @return
  /// std::nullopt: If the user never needs to be prompted in the first place.
  ///
  /// Otherwise:
  /// Information regarding the user's interaction with the save prompt and
  /// the subsequent save operation. The values should be interpreted as follows:
  ///
  /// userProceededWithSaveAttempt (first):
  ///   - std::nullopt: Canceled the save operation
  ///   - true: The user attempted to save the current account book.
  ///   - false: The user decided to proceed without saving
  /// saveSuccessfulResult:
  ///   - std::nullopt: The user canceled creating a file name for the account book that didn't have a
  ///   previously-determined path (ex. an account book that was recently
  ///       created but not saved).
  ///   - true: save was successful.
  ///   - false: save was unsuccessful
  auto promptUserToSaveIfAccountBookIsCurrentlyModified()
    -> std::optional< std::pair< std::optional< bool > /* userProceededWithSaveOperation */,
                                 std::optional< bool > /* successfuleSaveOpt */ > >;

  /// @pre filePath is a valid path candidate.
  /// @return true if save was successful; false otherwise.
  auto saveAccountBookInternal( std::filesystem::path const& filePath ) -> bool;

  /// Update internal handle and set modified to false.
  void updateAccountBookHandle( std::shared_ptr< dbsc::AccountBook > );
  void updateWindowTitle( bool accountBookIsModified );

  class Private;
  std::unique_ptr< Private > mImp;
};
} // namespace dbscqt

#endif // header include guard
