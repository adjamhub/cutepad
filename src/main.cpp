/*
 * Copyright (C) Andrea Diamantini 2020-2021 <adjam@protonmail.com>
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
    if ( !QDBusConnection::sessionBus().registerService( QStringLiteral("org.adjam.cutepad") ) )
    {
        qDebug() << "cutepad instance running. Connecting via dbus...";
        QDBusInterface iface( QStringLiteral("org.adjam.cutepad"), QStringLiteral("/App"), QStringLiteral("org.adjam.cutepad"), QDBusConnection::sessionBus());
        if (!iface.isValid()) {
            qDebug() << "invalid interface. Closing...";
            return 0;
        }

        QStringList paths;
        for (int i = 1; i < argc; i++) {
            QLatin1String p(argv[i]);
            paths << p;
        }
        QDBusMessage msg = iface.call( QStringLiteral("loadPaths") , paths);
        qDebug() << "error name:" << msg.errorName();
        return 0;
    }

    Application app(argc,argv);
    
    QCoreApplication::setApplicationName( QStringLiteral(PROJECT_NAME) );
    QCoreApplication::setApplicationVersion( QStringLiteral(PROJECT_VERSION) );
    QCoreApplication::setOrganizationName( QStringLiteral("adjam") );
    QCoreApplication::setOrganizationDomain( QStringLiteral("adjam.org") );

    app.parseCommandlineArgs();

    return app.exec();
}
