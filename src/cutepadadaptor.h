/*
 * Copyright (C) Andrea Diamantini 2020-2021 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#ifndef CUTEPAD_ADAPTOR_H
#define CUTEPAD_ADAPTOR_H


#include "application.h"

#include <QDBusAbstractAdaptor>


class CutepadAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.adjam.cutepad")

public:
    CutepadAdaptor (Application* app);

public slots:    
    Q_SCRIPTABLE void loadPaths(const QStringList &paths);

private:
    Application* _app;
};


#endif // CUTEPAD_ADAPTOR_H
