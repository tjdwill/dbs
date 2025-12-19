// dbscqt_mainwindow.cpp
#include "dbscqt_mainwindow.h"

#include <dbsc_accountbook.h>
#include <dbsc_dbscserializer.h>
#include <dbsc_tomlserializer.h>
#include <dbscqt_accountbookwidget.h>

#include <QtCore/QPointer>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <ui_dbscqt_mainwindow.h>

namespace dbscqt {
namespace {

  QString const kRecentAccountBookPathKey { "state/recentAccountBook" };
  /// Used to detect if the program shutdown via crash.
  QString const kAccountBookStateKey { "state/AccountBookUpToDate" };
  QString const kWindowSettingsKey { "window/Geometry" };
} // namespace
} // namespace dbscqt
