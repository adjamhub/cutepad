/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#include "application.h"

#include "config.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>

#include <QStringList>

#include <QDebug>


int main(int argc, char *argv[])
{
    if ( !QDBusConnection::sessionBus().registerService("org.adjam.cutepad") )
    {
        qDebug() << "cutepad instance running. Connecting via dbus...";
        QDBusInterface iface("org.adjam.cutepad", "/App", "org.adjam.cutepad", QDBusConnection::sessionBus());
        if (!iface.isValid()) {
            qDebug() << "invalid interface. Closing...";
            return 0;
        }

        QStringList paths;
        for (int i = 1; i < argc; i++) {
            QString p(argv[i]);
            paths << p;
        }
        QDBusMessage msg = iface.call("loadPaths", paths);
        qDebug() << "error name:" << msg.errorName();
        return 0;
    }

    Application app(argc,argv);
    
    app.setApplicationName(PROJECT_NAME);
    app.setApplicationVersion(PROJECT_VERSION);
    app.setOrganizationName("adjam");
    app.setOrganizationDomain("adjam.org");

    app.parseCommandlineArgs();

    return app.exec();
}
