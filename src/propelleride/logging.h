#pragma once

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(logmain)
Q_DECLARE_LOGGING_CATEGORY(logmainwindow)
Q_DECLARE_LOGGING_CATEGORY(logeditor)

Q_DECLARE_LOGGING_CATEGORY(logfilemanager)
Q_DECLARE_LOGGING_CATEGORY(logfile)

Q_DECLARE_LOGGING_CATEGORY(logbuildmanager)

Q_DECLARE_LOGGING_CATEGORY(loglanguage)
Q_DECLARE_LOGGING_CATEGORY(loghighlighter)

Q_DECLARE_LOGGING_CATEGORY(logcolorscheme)

void messageHandler(QtMsgType type,
        const QMessageLogContext &context,
        const QString &msg);
