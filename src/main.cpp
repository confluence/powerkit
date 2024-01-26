/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) Ole-André Rodlie <https://github.com/rodlie> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include <QApplication>
#include "powerkit_systray.h"
#include "powerkit_dialog.h"
#include "powerkit.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("freedesktop");
    QCoreApplication::setOrganizationDomain("org");
#ifdef POWERKIT_VERSION
    QString version = POWERKIT_VERSION;
#else
    QString version = "devel";
#endif
    QCoreApplication::setApplicationVersion(version);

    // launch config?
    QStringList args = QApplication::arguments();
    if (args.contains("--config")) {
        Dialog dialog;
        dialog.show();
        return a.exec();
    }

    // check if a powerkit session is already running
    QDBusInterface session(POWERKIT_SERVICE,
                           POWERKIT_PATH,
                           POWERKIT_SERVICE,
                           QDBusConnection::sessionBus());
    if (session.isValid()) {
        qWarning() << QObject::tr("A powerkit session is already running");
        return 1;
    }

    // start powerkit systray
    SysTray tray(a.parent());
    return a.exec();
}
