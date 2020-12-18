/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#include "application.h"

#include "version.h"


int main(int argc, char *argv[])
{
    Application app(argc,argv);
    
    app.setApplicationName("cutepad");
    app.setApplicationVersion(CUTEPAD_VERSION_STRING);
    app.setOrganizationName("adjam");
    app.setOrganizationDomain("adjam.org");

    app.parseCommandlineArgs();

    return app.exec();
}
