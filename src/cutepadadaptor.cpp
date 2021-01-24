/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0+ <https://spdx.org/licenses/GPL-3.0+.html>
 */


#include "cutepadadaptor.h"

#include <QDBusConnection>


CutepadAdaptor::CutepadAdaptor (Application* app)
    : QDBusAbstractAdaptor(app)
    , _app(app)
{
    QDBusConnection::sessionBus().registerObject( "/App" , this, QDBusConnection::ExportScriptableSlots);
}


void CutepadAdaptor::loadPaths(const QStringList &paths)
{
    _app->loadPaths(paths);
}
