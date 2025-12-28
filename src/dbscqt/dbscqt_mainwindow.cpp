// dbscqt_mainwindow.cpp
#include "dbscqt_mainwindow.h"

#include <dbsc_accountbook.h>
#include <dbsc_dbscserializer.h>
#include <dbsc_tomlserializer.h>
#include <dbscqt_accountbookwidget.h>
#include <dbscqt_generalpreferenceswidget.h>
#include <dbscqt_preferencekeys.h>
#include <dbscqt_preferenceswidget.h>

#include <bsls_assert.h>

#include <Qt> // Qt enums (Qt::ColorScheme)
#include <QtCore/QFileInfo>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QCloseEvent>
#include <QtGui/QGuiApplication>
#include <QtGui/QStyleHints>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTextBrowser>
#include <ui_dbscqt_mainwindow.h>

#include <exception>
#include <memory>
#include <optional>
#include <stdexcept>

namespace dbscqt {

namespace {

  QString const kOpenFileDialogFilter { QObject::tr( "Toml files (*.toml)" ) };
  QString const kBaseWindowTitle { QObject::tr( "Digital Budgeting System" ) };
} // namespace

class MainWindow::Private
{
public:
  ::Ui::MainWindow mUi {};
  std::shared_ptr< dbsc::AccountBook > mCurrentAccountBookHandle {};
  std::optional< std::filesystem::path > mPathToAccountBookFileOpt {};
  bool mCurrentAccountBookIsModified { false };
  bool mMainWindowIsClosing { false };
};

} // namespace dbscqt

dbscqt::MainWindow::MainWindow( QWidget* parent )
  : QMainWindow( parent )
  , mImp( std::make_unique< dbscqt::MainWindow::Private >() )
{
  mImp->mUi.setupUi( this );
  {
    // Set action connections
    QObject::connect( mImp->mUi.mCloseAction, &QAction::triggered, this, &dbscqt::MainWindow::closeAccountBook );
    QObject::connect( mImp->mUi.mNewAction, &QAction::triggered, this, &dbscqt::MainWindow::createNewAccountBook );
    QObject::connect(
      mImp->mUi.mOpenAction, &QAction::triggered, this, &dbscqt::MainWindow::handleOpenAccountBookTriggered );
    QObject::connect( mImp->mUi.mSaveAction, &QAction::triggered, this, &dbscqt::MainWindow::saveAccountBook );
    QObject::connect( mImp->mUi.mSaveAsAction, &QAction::triggered, this, &dbscqt::MainWindow::saveAccountBookAs );
    QObject::connect( mImp->mUi.mExitAction, &QAction::triggered, this, &dbscqt::MainWindow::attemptExitProgram );
    QObject::connect( mImp->mUi.mAboutAction, &QAction::triggered, this, &dbscqt::MainWindow::showAboutPage );
    QObject::connect( mImp->mUi.mAboutQtAction, &QAction::triggered, this, &dbscqt::MainWindow::showAboutQtPage );
    QObject::connect(
      this, &dbscqt::MainWindow::accountBookLoaded, [this]( std::shared_ptr< dbsc::AccountBook > accountBookHandle ) {
        bool const accountBookIsLoaded = static_cast< bool >( accountBookHandle );
        mImp->mUi.mSaveAction->setEnabled( accountBookIsLoaded );
        mImp->mUi.mSaveAsAction->setEnabled( accountBookIsLoaded );
        mImp->mUi.mCloseAction->setEnabled( accountBookIsLoaded );
      } );
    QObject::connect(
      mImp->mUi.mPreferencesAction, &QAction::triggered, this, &dbscqt::MainWindow::openPreferencesWindow );
  }

  // Attempt to load the most recently-used account book
  /// TODO: Add a check to determine if the program shutdown properly in the previous
  /// session.
  auto const mostRecentAccountBookPath =
    QSettings().value( dbscqt::PreferenceKeys::kRecentAccountBookPathKey, QString() ).toString();
  mImp->mPathToAccountBookFileOpt = mostRecentAccountBookPath.isEmpty()
                                    ? std::nullopt
                                    : std::optional< std::filesystem::path >( mostRecentAccountBookPath.toStdString() );
  if ( mImp->mPathToAccountBookFileOpt.has_value() ) {
    loadAccountBook( mImp->mPathToAccountBookFileOpt.value() );
  }

  // Configure the account book widget. This is done after the most
  // recently-used book is loaded to take advantage of the AccountBookWidget's
  // constructor that takes a shared_ptr to the account book to initialize
  // itself and its children.
  auto* accountBookWidget = new dbscqt::AccountBookWidget( mImp->mCurrentAccountBookHandle );
  {
    QObject::connect( this,
                      &dbscqt::MainWindow::accountBookLoaded,
                      accountBookWidget,
                      &dbscqt::AccountBookWidget::handleAccountBookSet );
    QObject::connect( accountBookWidget, &dbscqt::AccountBookWidget::accountBookModified, this, [this]() {
      handleAccountBookModified( true );
    } );

    auto* accountBookDisplayLayout = new QVBoxLayout( mImp->mUi.mAccountBookDisplayPage );
    accountBookDisplayLayout->setContentsMargins( {} );
    accountBookDisplayLayout->addWidget( accountBookWidget );
  }

  // Window's initial state.
  restoreGeometry( QSettings().value( dbscqt::PreferenceKeys::kWindowGeometryKey ).toByteArray() );
  restoreState( QSettings().value( dbscqt::PreferenceKeys::kWindowStateKey ).toByteArray() );
  if ( mImp->mCurrentAccountBookHandle ) {
    mImp->mUi.mStackedWidget->setCurrentWidget( mImp->mUi.mAccountBookDisplayPage );
  } else {
    mImp->mUi.mStackedWidget->setCurrentWidget( mImp->mUi.mWelcomePage );
  }

  auto* preferencesWidget = createPreferencesWidget( this );
  preferencesWidget->applyAll();
  preferencesWidget->deleteLater();
}

dbscqt::MainWindow::~MainWindow() = default;

void dbscqt::MainWindow::closeEvent( QCloseEvent* event )
{
  /// Intercept the close event to execute save logic.
  if ( !mImp->mMainWindowIsClosing && !attemptExitProgram() ) {
    event->ignore();
  }
}

auto dbscqt::MainWindow::attemptExitProgram() -> bool
{
  if ( shouldAbortOperation() ) {
    return false;
  }

  QSettings().setValue( dbscqt::PreferenceKeys::kWindowGeometryKey, saveGeometry() );
  QSettings().setValue( dbscqt::PreferenceKeys::kWindowStateKey, saveState() );
  mImp->mMainWindowIsClosing = true;
  close();
  return true;
}

auto dbscqt::MainWindow::closeAccountBook() -> bool
{
  if ( shouldAbortOperation() ) {
    return false;
  }

  mImp->mPathToAccountBookFileOpt = std::nullopt;
  updateAccountBookHandle( nullptr );
  handleAccountBookModified( false );
  mImp->mUi.mStackedWidget->setCurrentWidget( mImp->mUi.mWelcomePage );
  QSettings().setValue( dbscqt::PreferenceKeys::kRecentAccountBookPathKey, QString() );
  return true;
}

void dbscqt::MainWindow::createNewAccountBook()
{
  if ( shouldAbortOperation() ) {
    return;
  }

  bool userAccepted {};
  QString const accountBookOwner =
    QInputDialog::getText( this, "Create an account book", "Owner Name:", QLineEdit::Normal, QString(), &userAccepted );
  if ( userAccepted && !accountBookOwner.isEmpty() ) {
    mImp->mPathToAccountBookFileOpt = std::nullopt; // No save path yet.
    updateAccountBookHandle( std::make_shared< dbsc::AccountBook >( accountBookOwner.toStdString() ) );
    handleAccountBookModified( true );
    // Show display page if it isn't already active.
    mImp->mUi.mStackedWidget->setCurrentWidget( mImp->mUi.mAccountBookDisplayPage );
  }
}

void dbscqt::MainWindow::handleAccountBookModified( bool const isModified )
{
  mImp->mCurrentAccountBookIsModified = isModified;
  QSettings().setValue( dbscqt::PreferenceKeys::kAccountBookIsCurrentlyModified, isModified );
  mImp->mUi.mSaveAction->setEnabled( isModified );
  updateWindowTitle( isModified );
}

void dbscqt::MainWindow::handleOpenAccountBookTriggered()
{
  if ( shouldAbortOperation() ) {
    return;
  }

  auto const startingDirectory =
    QSettings().value( dbscqt::PreferenceKeys::kLastAccountBookFileDirectoryKey, "/home" ).toString();
  auto const userSelectedFilePath =
    QFileDialog::getOpenFileName( this, "Open an account book", startingDirectory, dbscqt::kOpenFileDialogFilter );

  auto const filePathOpt =
    userSelectedFilePath.isEmpty()
      ? std::nullopt
      : std::optional< std::filesystem::path >( QFileInfo( userSelectedFilePath ).filesystemCanonicalFilePath() );

  if ( filePathOpt.has_value() ) {

    std::filesystem::path const selectedFilePath { userSelectedFilePath.toStdString() };
    std::filesystem::path const selectedFileParentDirectory { selectedFilePath.parent_path() };
    QSettings().setValue( dbscqt::PreferenceKeys::kLastAccountBookFileDirectoryKey,
                          QString::fromStdString( selectedFileParentDirectory.string() ) );
    loadAccountBook( filePathOpt.value() );

  } else {
    // The user pressed cancel when selecting a file. No-op.
  }
}

void dbscqt::MainWindow::openPreferencesWindow()
{
  auto* preferencesWindow = dbscqt::MainWindow::createPreferencesWidget( this );
  preferencesWindow->setParent( this );
  QObject::connect(
    preferencesWindow, &QDialog::finished, [preferencesWindow]() { preferencesWindow->deleteLater(); } );

  preferencesWindow->show();
}

auto dbscqt::MainWindow::saveAccountBook() -> std::optional< bool >
{
  if ( !mImp->mPathToAccountBookFileOpt.has_value() ) {
    return saveAccountBookAs();
  }

  // Use the currently cached file path.
  bool const saveWasSuccessful = saveAccountBookInternal( mImp->mPathToAccountBookFileOpt.value() );
  if ( saveWasSuccessful ) {
    handleAccountBookModified( false );
    QSettings().setValue( dbscqt::PreferenceKeys::kRecentAccountBookPathKey,
                          QString::fromStdString( mImp->mPathToAccountBookFileOpt.value().string() ) );
  }

  return saveWasSuccessful;
}

auto dbscqt::MainWindow::saveAccountBookAs() -> std::optional< bool >
{

  std::optional< bool > saveWasSuccessfulOpt;
  QString const userSelectedSaveLocation = QFileDialog::getSaveFileName(
    this,
    "Save the current account book",
    QSettings().value( dbscqt::PreferenceKeys::kLastAccountBookFileDirectoryKey, QString() ).toString(),
    dbscqt::kOpenFileDialogFilter );
  if ( !userSelectedSaveLocation.isEmpty() ) {
    // Update QSettings directory key.
    std::filesystem::path const selectedFilePath { userSelectedSaveLocation.toStdString() };
    std::filesystem::path const selectedFileParentDirectory { selectedFilePath.parent_path() };
    QSettings().setValue( dbscqt::PreferenceKeys::kLastAccountBookFileDirectoryKey,
                          QString::fromStdString( selectedFileParentDirectory.string() ) );

    // Write the file.
    bool const saveWasSuccessful = saveAccountBookInternal( selectedFilePath );
    if ( saveWasSuccessful ) {
      mImp->mPathToAccountBookFileOpt = selectedFilePath;
      QSettings().setValue( dbscqt::PreferenceKeys::kRecentAccountBookPathKey, userSelectedSaveLocation );
      handleAccountBookModified( false );
    }
    saveWasSuccessfulOpt = saveWasSuccessful;
  }

  return saveWasSuccessfulOpt;
}

void dbscqt::MainWindow::showAboutPage()
{
  auto aboutPage       = QPointer( new QDialog() );
  auto aboutPageLayout = QPointer( new QVBoxLayout( aboutPage ) );
  QObject::connect( aboutPage, &QDialog::finished, [aboutPage]() { aboutPage->deleteLater(); } );

  QString markdownText {};
  QTextStream tss { &markdownText };
  tss << R"(
  # Digital Budgeting System

  Copyright &copy; 2025 Terrance Williams (tjdwill)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

  &nbsp;

  ## Libraries

  The following libraries are used in this application:

  - [bde](https://github.com/bloomberg/bde) by Bloomberg
  - [Qt](https://qt.io) (v6.9.x) by Qt Group (The Qt Company)
  - [stduuid](https://github.com/mariusbancila/stduuid) by @mariusbancila (v1.2.3)
  - [tomlplusplus](https://github.com/marzer/tomlplusplus) by @marzer (v3.4.0)

  ## Icons

  - [Exchange](https://icons8.com/icon/61743/exchange) icon by [Icons8](https://icons8.com) 
  - [Plus](https://icons8.com/icon/1501/plus) icon by [Icons8](https://icons8.com) 
  - [Save](https://icons8.com/icon/18765/save) icon by [Icons8](https://icons8.com) 
  - [Save as](https://icons8.com/icon/3701/save-as) icon by [Icons8](https://icons8.com) 
  - [Settings](https://icons8.com/icon/364/settings) icon by [Icons8](https://icons8.com) 
  - [Toggle Indeterminate](https://icons8.com/icon/123425/toggle-indeterminate) icon by [Icons8](https://icons8.com) 
  - [U.S. Dollar Sign](https://icons8.com/icon/7172/us-dollar-circled) icon by [Icons8](https://icons8.com) 

  )";
  auto aboutPageTextDisplay = QPointer( new QTextBrowser() );
  aboutPageTextDisplay->setOpenLinks( false );
  // aboutPageTextDisplay->setOpenExternalLinks( true );
  aboutPageTextDisplay->setMarkdown( markdownText );
  aboutPageTextDisplay->setReadOnly( true );
  aboutPageLayout->addWidget( aboutPageTextDisplay );
  constexpr int kMinHeight { 600 };
  constexpr int kMinWidth { 800 };
  aboutPage->setMinimumSize( { kMinWidth, kMinHeight } );

  aboutPage->open();
}

void dbscqt::MainWindow::showAboutQtPage()
{
  QMessageBox::aboutQt( this, "About Qt" );
}

auto dbscqt::MainWindow::createPreferencesWidget( QWidget* parent ) -> dbscqt::PreferencesWidget*
{
  auto* preferencesWindow = new dbscqt::PreferencesWidget( parent );
  preferencesWindow->addPreferencePage( new dbscqt::GeneralPreferencesWidget() );

  return preferencesWindow;
}

void dbscqt::MainWindow::loadAccountBook( std::filesystem::path const& accountBookFile )
{
  auto const loadAccountBookInternal =
    [this]( std::filesystem::path const& accountBookFilePath ) -> std::shared_ptr< dbsc::AccountBook > {
    try {
      return std::make_shared< dbsc::AccountBook >(
        dbsc::readAccountBook< dbsc::TomlSerializer >( accountBookFilePath ) );
    } catch ( std::exception const& error ) {
      auto const errorMessage = QString( "Could not load the account book %1 due to the following error:\n\n%2" )
                                  .arg( accountBookFilePath.string() )
                                  .arg( error.what() );
      QMessageBox::warning( this, "File load error", errorMessage );
      return nullptr;
    }
  };
  auto const loadedAccountBook = loadAccountBookInternal( accountBookFile );

  if ( loadedAccountBook ) {
    mImp->mPathToAccountBookFileOpt = accountBookFile;
    QSettings().setValue( dbscqt::PreferenceKeys::kRecentAccountBookPathKey,
                          QString::fromStdString( mImp->mPathToAccountBookFileOpt.value().string() ) );
    updateAccountBookHandle( loadedAccountBook );
    handleAccountBookModified( false );
    mImp->mUi.mStackedWidget->setCurrentWidget( mImp->mUi.mAccountBookDisplayPage );
  }
}

auto dbscqt::MainWindow::promptUserToSaveIfAccountBookIsCurrentlyModified()
  -> std::optional< std::pair< std::optional< bool > /* userProceededWithSaveOperation */,
                               std::optional< bool > /* successfulSaveOpt */ > >
{

  if ( mImp->mCurrentAccountBookIsModified ) {
    std::optional< bool > userProceededWithSaveOperation {};
    std::optional< bool > successfulSaveOpt {};
    auto const buttonPressed =
      QMessageBox::question( this,
                             "Save Prompt",
                             "This account book currently has unsaved data. Save the file?",
                             QMessageBox::StandardButtons( QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel ),
                             QMessageBox::Cancel );

    switch ( buttonPressed ) {
      case QMessageBox::Yes:
        userProceededWithSaveOperation = true;
        successfulSaveOpt              = saveAccountBook();
        break;
      case QMessageBox::No:
        userProceededWithSaveOperation = false;
        break;
      case QMessageBox::Cancel:
        // userProceededWithSaveOperation = std::nullopt;
        // successfulSaveOpt = std::nullopt;
        break;
      default:
        throw std::runtime_error( "Unhandled, unexpected QMessageBox::StandardButton enum variant." );
    }
    return std::optional< std::pair< std::optional< bool >, std::optional< bool > > > {
      { userProceededWithSaveOperation, successfulSaveOpt }
    };
  }
  return std::nullopt;
}

auto dbscqt::MainWindow::saveAccountBookInternal( std::filesystem::path const& filePath ) -> bool
{
  BSLS_ASSERT( mImp->mCurrentAccountBookHandle );

  bool operationSuccessful = true;

  try {
    dbsc::writeAccountBook< dbsc::TomlSerializer >( *mImp->mCurrentAccountBookHandle, filePath );
  } catch ( std::exception const& error ) {
    QMessageBox::warning( this,
                          "Error saving file",
                          QString( "Could not save the current account book %1. Reason:\n\n%2" )
                            .arg( filePath.string() )
                            .arg( error.what() ) );
    operationSuccessful = false;
  }

  return operationSuccessful;
}

auto dbscqt::MainWindow::shouldAbortOperation() -> bool
{
  auto const userPromptedResultsOpt = promptUserToSaveIfAccountBookIsCurrentlyModified();
  bool const userNeededPrompting    = userPromptedResultsOpt.has_value();
  if ( userNeededPrompting ) {
    auto const [userSavePromptResult, saveAttemptResult] = userPromptedResultsOpt.value();
    bool const userAttemptedToSave                       = userSavePromptResult == true;
    bool const attemptedSaveWasUnsuccessful              = userAttemptedToSave && ( saveAttemptResult != true );
    bool const userCanceledOperation                     = !userSavePromptResult.has_value();
    if ( attemptedSaveWasUnsuccessful || userCanceledOperation ) {
      return true;
    }
  }
  return false;
}

void dbscqt::MainWindow::updateAccountBookHandle( std::shared_ptr< dbsc::AccountBook > newHandle )
{
  mImp->mCurrentAccountBookHandle = newHandle;
  Q_EMIT accountBookLoaded( mImp->mCurrentAccountBookHandle );
}

void dbscqt::MainWindow::updateWindowTitle( bool const accountBookIsModified )
{

  if ( mImp->mCurrentAccountBookHandle ) {
    QString const filePath = mImp->mPathToAccountBookFileOpt.has_value()
                             ? QString::fromStdString( mImp->mPathToAccountBookFileOpt.value().string() )
                             : "Unsaved";
    QString const windowTitleWithAccountOwnerName = QString( "%1 - %2 (%3)" )
                                                      .arg( dbscqt::kBaseWindowTitle )
                                                      .arg( mImp->mCurrentAccountBookHandle->owner() )
                                                      .arg( filePath );
    MainWindow::setWindowTitle( accountBookIsModified ? QString( "%1*" ).arg( windowTitleWithAccountOwnerName )
                                                      : windowTitleWithAccountOwnerName );
  } else {
    MainWindow::setWindowTitle( accountBookIsModified ? QString( "%1*" ).arg( dbscqt::kBaseWindowTitle )
                                                      : dbscqt::kBaseWindowTitle );
  }
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
