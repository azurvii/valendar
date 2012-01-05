/*
 * main.cpp
 *
 *  Created on: Oct 16, 2011
 *      Author: vincentliu
 */

#include <QApplication>
#include "GalendarForm.h"

int main(int argc, char **argv) {
	QApplication a(argc, argv);
	a.setOrganizationName("azurvii");
	a.setOrganizationDomain("azurvii.net");
	a.setApplicationName("Galendar");
	a.setApplicationVersion("1.0");
	GalendarForm w;
	w.show();
	return a.exec();
}
