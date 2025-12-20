// dbscqt_mainwindow.cpp
#include "dbscqt_mainwindow.h"

#include "Qt/6.9.1/gcc_64/include/QtWidgets/qmessagebox.h"

#include <dbsc_accountbook.h>
#include <dbsc_dbscserializer.h>
#include <dbsc_tomlserializer.h>
#include <dbscqt_accountbookwidget.h>

#include <bsls_assert.h>

#include <QtCore/QFileInfo>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMessageBox>
#include <ui_dbscqt_mainwindow.h>

#include <memory>
#include <optional>
#include <stdexcept>

namespace dbscqt {

namespace {

  QString const kRecentAccountBookPathKey { "state/recentAccountBook" };
  /// Used to detect if the program shutdown via crash.
  QString const kAccountBookIsCurrentlyModified { "state/AccountBookUpToDate" };
  QString const kWindowGeometryKey { "window/Geometry" };
  QString const kWindowStateKey { "window/State" };
  QString const kLastAccountBookFileDirectoryKey { "state/LastAccountBookFileDirectory" };

  QString const kOpenFileDialogFilter { QObject::tr( "Toml files (*.toml)" ) };
} // namespace

class MainWindow::Private
{
public:
  ::Ui::MainWindow mUi {};
  std::shared_ptr< dbsc::AccountBook > mCurrentAccountBookHandle {};
  std::optional< std::filesystem::path > mPathToAccountBookFileOpt {};
  bool mCurrentAccountBookIsModified { false };
};

} // namespace dbscqt

dbscqt::MainWindow::MainWindow( QWidget* parent )
  : QMainWindow( parent )
  , mImp( std::make_unique< dbscqt::MainWindow::Private >() )
{
  // Set action connections
  QObject::connect( mImp->mUi.mCloseAction, &QAction::triggered, this, &dbscqt::MainWindow::closeAccountBook );
  QObject::connect( mImp->mUi.mNewAction, &QAction::triggered, this, &dbscqt::MainWindow::createNewAccountBook );
  QObject::connect(
    mImp->mUi.mOpenAction, &QAction::triggered, this, &dbscqt::MainWindow::handleOpenAccountBookTriggered );
  QObject::connect( mImp->mUi.mSaveAction, &QAction::triggered, this, &dbscqt::MainWindow::saveAccountBook );
  QObject::connect( mImp->mUi.mExitAction, &QAction::triggered, this, &dbscqt::MainWindow::exitProgram );
  // QObject::connect( mImp->mUi.mAboutAction, &QAction::triggered, this, &dbscqt::MainWindow::showAboutPage );
  QObject::connect( mImp->mUi.mAboutQtAction, &QAction::triggered, this, &dbscqt::MainWindow::showAboutQtPage );
  QObject::connect(
    this, &dbscqt::MainWindow::accountBookLoaded, [this]( std::shared_ptr< dbsc::AccountBook > accountBookHandle ) {
      bool const accountBookIsLoaded = static_cast< bool >( accountBookHandle );
      mImp->mUi.mSaveAction->setEnabled( accountBookIsLoaded );
      mImp->mUi.mCloseAction->setEnabled( accountBookIsLoaded );
    } );

  // Attempt to load the most recently-used account book
  /// TODO: Add a check to determine if the program shutdown properly in the previous
  /// session.
  auto const mostRecentAccountBookPath = QSettings().value( dbscqt::kRecentAccountBookPathKey, QString() ).toString();
  mImp->mPathToAccountBookFileOpt      = mostRecentAccountBookPath.isEmpty()
                                         ? std::nullopt
                                         : std::optional< std::filesystem::path >( mostRecentAccountBookPath.toStdString() );
  if ( mImp->mPathToAccountBookFileOpt.has_value() ) {
    loadAccountBookInternal( mImp->mPathToAccountBookFileOpt.value() );
  }

  // Configure the account book widget
  mImp->mUi.setupUi( this );
  {
    auto* accountBookWidget = new dbscqt::AccountBookWidget( mImp->mCurrentAccountBookHandle );
    QObject::connect( this,
                      &dbscqt::MainWindow::accountBookLoaded,
                      accountBookWidget,
                      &dbscqt::AccountBookWidget::handleAccountBookSet );
    QObject::connect( accountBookWidget, &dbscqt::AccountBookWidget::accountBookModified, this, [this]() {
      handleAccountBookModified( true );
    } );

    mImp->mUi.mAccountBookDisplayPage->setLayout( new QVBoxLayout() );
    mImp->mUi.mAccountBookDisplayPage->layout()->addWidget( accountBookWidget );
  }

  // Window's initial state.
  restoreGeometry( QSettings().value( dbscqt::kWindowGeometryKey ).toByteArray() );
  restoreState( QSettings().value( dbscqt::kWindowStateKey ).toByteArray() );
}

dbscqt::MainWindow::~MainWindow() = default;

auto dbscqt::MainWindow::closeAccountBook() -> bool
{
  auto const [userSavePromptResult, saveAttemptResult] = promptUserToSaveIfAccountBookIsCurrentlyModified();
  bool const userAttemptedToSave                       = userSavePromptResult == true;
  bool const attemptedSaveWasUnsuccessful              = userAttemptedToSave && ( saveAttemptResult != true );
  bool const userCanceledOperation                     = !userSavePromptResult.has_value();
  if ( attemptedSaveWasUnsuccessful || userCanceledOperation ) {
    return false;
  }

  mImp->mPathToAccountBookFileOpt = std::nullopt;
  updateAccountBookHandle( nullptr );
  handleAccountBookModified( false );
  mImp->mUi.mStackedWidget->setCurrentWidget( mImp->mUi.mWelcomePage );
  return true;
}

void dbscqt::MainWindow::createNewAccountBook()
{
  auto const [userSavePromptResult, saveAttemptResult] = promptUserToSaveIfAccountBookIsCurrentlyModified();
  bool const userAttemptedToSave                       = userSavePromptResult == true;
  bool const attemptedSaveWasUnsuccessful              = userAttemptedToSave && ( saveAttemptResult != true );
  bool const userCanceledOperation                     = !userSavePromptResult.has_value();
  if ( attemptedSaveWasUnsuccessful || userCanceledOperation ) {
    return;
  }

  bool userAccepted {};
  QString const accountBookOwner =
    QInputDialog::getText( this, "Create an account book", "Owner Name:", QLineEdit::Normal, QString(), &userAccepted );
  if ( userAccepted && !accountBookOwner.isEmpty() ) {
    mImp->mPathToAccountBookFileOpt = std::nullopt; // No save path yet.
    handleAccountBookModified( false );
    updateAccountBookHandle( std::make_shared< dbsc::AccountBook >( accountBookOwner.toStdString() ) );
    // Show display page if it isn't already active.
    mImp->mUi.mStackedWidget->setCurrentWidget( mImp->mUi.mAccountBookDisplayPage );
  }
}

void dbscqt::MainWindow::exitProgram()
{
  auto const [userSavePromptResult, saveAttemptResult] = promptUserToSaveIfAccountBookIsCurrentlyModified();
  bool const userAttemptedToSave                       = userSavePromptResult == true;
  bool const attemptedSaveWasUnsuccessful              = userAttemptedToSave && ( saveAttemptResult != true );
  bool const userCanceledOperation                     = !userSavePromptResult.has_value();
  if ( attemptedSaveWasUnsuccessful || userCanceledOperation ) {
    return;
  }

  QSettings().setValue( dbscqt::kWindowGeometryKey, saveGeometry() );
  QSettings().setValue( dbscqt::kWindowStateKey, saveState() );
  Q_EMIT shutdownInitiated();
}

void dbscqt::MainWindow::handleAccountBookModified( bool const isModified )
{
  mImp->mCurrentAccountBookIsModified = isModified;
  QSettings().setValue( dbscqt::kAccountBookIsCurrentlyModified, isModified );
  updateWindowTitle( isModified );
}

void dbscqt::MainWindow::handleOpenAccountBookTriggered()
{

  auto const startingDirectory = QSettings().value( dbscqt::kLastAccountBookFileDirectoryKey, "/home" ).toString();
  auto const userSelectedFilePath =
    QFileDialog::getOpenFileName( this, "Open an account book", startingDirectory, dbscqt::kOpenFileDialogFilter );

  auto const filePathOpt =
    userSelectedFilePath.isEmpty()
      ? std::nullopt
      : std::optional< std::filesystem::path >( QFileInfo( userSelectedFilePath ).filesystemCanonicalFilePath() );

  if ( filePathOpt.has_value() ) {
    auto const [userSavePromptResult, saveAttemptResult] = promptUserToSaveIfAccountBookIsCurrentlyModified();
    bool const userAttemptedToSave                       = userSavePromptResult == true;
    bool const attemptedSaveWasUnsuccessful              = userAttemptedToSave && ( saveAttemptResult != true );
    bool const userCanceledOperation                     = !userSavePromptResult.has_value();
    if ( attemptedSaveWasUnsuccessful || userCanceledOperation ) {
      return;
    }

    // Checkpoint: Save was successful or the user proceeded without saving
    QFileInfo const fileInfo { userSelectedFilePath };
    QSettings().setValue( dbscqt::kLastAccountBookFileDirectoryKey, fileInfo.canonicalPath() );
    auto const loadedAccountBook = loadAccountBookInternal( filePathOpt.value() );

    if ( loadedAccountBook ) {
      mImp->mPathToAccountBookFileOpt = filePathOpt;
      QSettings().setValue( dbscqt::kRecentAccountBookPathKey,
                            QString::fromStdString( mImp->mPathToAccountBookFileOpt.value().string() ) );
      handleAccountBookModified( false );
      updateAccountBookHandle( loadedAccountBook );
      mImp->mUi.mStackedWidget->setCurrentWidget( mImp->mUi.mAccountBookDisplayPage );
    }
  } else {
    // The user pressed cancel when selecting a file. No op.
  }
}

auto dbscqt::MainWindow::saveAccountBook() -> std::optional< bool >
{
  if ( !mImp->mPathToAccountBookFileOpt.has_value() ) {
    QString const userSelectedSaveLocation =
      QFileDialog::getSaveFileName( this,
                                    "Save the current account book",
                                    QSettings().value( dbscqt::kLastAccountBookFileDirectoryKey, QString() ).toString(),
                                    dbscqt::kOpenFileDialogFilter );
    if ( userSelectedSaveLocation.isEmpty() ) {
      return std::nullopt;
    }
    mImp->mPathToAccountBookFileOpt = userSelectedSaveLocation.toStdString();
  }

  bool const saveWasSuccessful = saveAccountBookInternal( mImp->mPathToAccountBookFileOpt.value() );
  if ( saveWasSuccessful ) {
    handleAccountBookModified( false );
  }

  return saveWasSuccessful;
}

void dbscqt::MainWindow::showAboutPage() {}

void dbscqt::MainWindow::showAboutQtPage()
{
  QMessageBox::aboutQt( this, "About Qt" );
}

auto dbscqt::MainWindow::loadAccountBookInternal( std::filesystem::path const& accountBookFile )
  -> std::shared_ptr< dbsc::AccountBook >
{
  try {
    return std::make_shared< dbsc::AccountBook >( dbsc::readAccountBook< dbsc::TomlSerializer >( accountBookFile ) );
  } catch ( dbsc::DbscSerializationException const& error ) {
    auto const errorMessage =
      QString( "Could not load the account book due to the following error: %1" ).arg( error.what() );
    QMessageBox::warning( this, "File load error", errorMessage );
    return nullptr;
  }
}

auto dbscqt::MainWindow::promptUserToSaveIfAccountBookIsCurrentlyModified()
  -> std::pair< std::optional< bool > /* userProceededWithSaveOperation */,
                std::optional< bool > /* successfuleSaveOpt */ >
{
  std::optional< bool > userProceededWithSaveOperation {};
  std::optional< bool > successfulSaveOpt {};

  if ( mImp->mCurrentAccountBookIsModified ) {
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
  }
  return { userProceededWithSaveOperation, successfulSaveOpt };
}

auto dbscqt::MainWindow::saveAccountBookInternal( std::filesystem::path const& filePath ) -> bool
{
  BSLS_ASSERT( mImp->mCurrentAccountBookHandle );

  bool operationSuccessful = true;

  try {
    dbsc::writeAccountBook< dbsc::TomlSerializer >( *mImp->mCurrentAccountBookHandle, filePath );
  } catch ( dbsc::DbscSerializationException const& error ) {
    QMessageBox::warning(
      this, "Error saving file", QString( "Could not save the current account book. Reason: %1" ).arg( error.what() ) );
    operationSuccessful = false;
  }

  return operationSuccessful;
}

void dbscqt::MainWindow::updateAccountBookHandle( std::shared_ptr< dbsc::AccountBook > newHandle )
{
  mImp->mCurrentAccountBookHandle = newHandle;
  Q_EMIT accountBookLoaded( mImp->mCurrentAccountBookHandle );
}

void dbscqt::MainWindow::updateWindowTitle( bool const accountBookIsModified )
{
  QMainWindow::setWindowTitle( accountBookIsModified ? "Digital Budgeting System (DBS)*"
                                                     : "Digital Budgeting System (DBS)" );
}
