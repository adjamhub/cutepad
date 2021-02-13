/*
 * Copyright (C) Andrea Diamantini 2020-2021 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#include "cutepadadaptor.h"
#include "application.h"

#include <QDBusConnection>


CutepadAdaptor::CutepadAdaptor (Application* app)
    : QDBusAbstractAdaptor(app)
    , _app(app)
{
    QDBusConnection::sessionBus().registerObject( QStringLiteral("/App") , this, QDBusConnection::ExportScriptableSlots);
}


void CutepadAdaptor::loadPaths(const QStringList &paths)
{
    _app->loadPaths(paths);
}
