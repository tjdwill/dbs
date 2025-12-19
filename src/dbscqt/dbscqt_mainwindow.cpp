// dbscqt_mainwindow.cpp
#include "dbscqt_mainwindow.h"

#include <dbsc_accountbook.h>
#include <dbsc_dbscserializer.h>
#include <dbsc_tomlserializer.h>
#include <dbscqt_accountbookwidget.h>

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

namespace dbscqt {

namespace {

  QString const kRecentAccountBookPathKey { "state/recentAccountBook" };
  /// Used to detect if the program shutdown via crash.
  QString const kAccountBookIsCurrentlyModified { "state/AccountBookUpToDate" };
  QString const kWindowSettingsKey { "window/Geometry" };
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
  // Attempt to load the most recently-used account book
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
}

dbscqt::MainWindow::~MainWindow() = default;

auto dbscqt::MainWindow::closeAccountBook() -> bool
{
  /// TODO: add save check

  mImp->mUi.mStackedWidget->setCurrentWidget( mImp->mUi.mWelcomePage );
  updateAccountBookHandle( nullptr );
  return true;
}

void dbscqt::MainWindow::createNewAccountBook()
{
  if ( closeAccountBook() ) {
    bool userAccepted {};
    QString const accountBookOwner = QInputDialog::getText(
      this, "Create an account book", "Owner Name:", QLineEdit::Normal, QString(), &userAccepted );
    if ( userAccepted && !accountBookOwner.isEmpty() ) {
      updateAccountBookHandle( std::make_shared< dbsc::AccountBook >( accountBookOwner.toStdString() ) );
    }
  }
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
  auto const filePath =
    QFileDialog::getOpenFileName( this, "Open an account book", startingDirectory, dbscqt::kOpenFileDialogFilter );

  mImp->mPathToAccountBookFileOpt =
    filePath.isEmpty() ? std::nullopt
                       : std::optional< std::filesystem::path >( QFileInfo( filePath ).filesystemCanonicalFilePath() );

  if ( mImp->mPathToAccountBookFileOpt.has_value() ) {
    QFileInfo const fileInfo { filePath };
    QSettings().setValue( dbscqt::kLastAccountBookFileDirectoryKey, fileInfo.canonicalPath() );
    loadAccountBookInternal( mImp->mPathToAccountBookFileOpt.value() );
  }
}

void dbscqt::MainWindow::loadAccountBookInternal( std::filesystem::path const& accountBookFile )
{
  std::shared_ptr< dbsc::AccountBook > loadedAccountBook;
  try {
    loadedAccountBook =
      std::make_shared< dbsc::AccountBook >( dbsc::readAccountBook< dbsc::TomlSerializer >( accountBookFile ) );
  } catch ( dbsc::DbscSerializationException const& error ) {
    auto const errorMessage =
      QString( "Could not load the account book due to the following error: %1" ).arg( error.what() );
    QMessageBox::warning( this, "File load error", errorMessage );
    return;
  }

  // Update Settings and application state
  QSettings().setValue( dbscqt::kRecentAccountBookPathKey, QString::fromStdString( accountBookFile.string() ) );
  mImp->mUi.mStackedWidget->setCurrentWidget( mImp->mUi.mAccountBookDisplayPage );
  updateAccountBookHandle( loadedAccountBook );
}

void dbscqt::MainWindow::updateAccountBookHandle( std::shared_ptr< dbsc::AccountBook > newHandle )
{
  mImp->mCurrentAccountBookHandle = newHandle;
  handleAccountBookModified( false );
  Q_EMIT accountBookLoaded( mImp->mCurrentAccountBookHandle );
}

void dbscqt::MainWindow::updateWindowTitle( bool const accountBookIsModified )
{
  QMainWindow::setWindowTitle( accountBookIsModified ? "Digital Budgeting System (DBS)*"
                                                     : "Digital Budgeting System (DBS)" );
}
