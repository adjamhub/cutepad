/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */
 

#include "version.h"

#include "application.h"
#include "mainwindow.h"


Application::Application(int &argc, char *argv[])
    : QApplication(argc,argv)
{
}


Application::~Application()
{
}


void Application::parseCommandlineArgs()
{
	MainWindow* w = new MainWindow();
	w->show();
}
