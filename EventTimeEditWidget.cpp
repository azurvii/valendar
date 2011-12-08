/*
 * EventTimeEditWidget.cpp
 *
 *  Created on: Dec 8, 2011
 *      Author: vincent
 */

#include "EventTimeEditWidget.h"
//#include <iostream>
//using namespace std;

EventTimeEditWidget::EventTimeEditWidget(bool backward, QWidget *parent) :
		QWidget(parent) {
	ui.setupUi(this);
//	cerr << "setAutoFillBackground" << endl;
	ui.frame->setAutoFillBackground(true);
	ui.backwardCheck->setVisible(backward);
}

void EventTimeEditWidget::setTime(int secs) {
//	cerr << "setTime" << endl;
	if (secs < 0) {
		secs = -secs;
		ui.backwardCheck->setChecked(true);
	} else {
		ui.backwardCheck->setChecked(false);
	}
	ui.daySpin->setValue(secs / 24 / 3600);
	ui.hourSpin->setValue((secs % (24 * 3600)) / 3600);
	ui.minuteSpin->setValue((secs % (3600)) / 60);
	ui.secondSpin->setValue(secs % 60);
}

void EventTimeEditWidget::on_hourSpin_valueChanged(int value) {
	int days = value / 24;
	if (days > 0) {
		ui.daySpin->setValue(ui.daySpin->value() + days);
		ui.hourSpin->setValue(value % 24);
	}
}

void EventTimeEditWidget::on_minuteSpin_valueChanged(int value) {
	int hours = value / 60;
	if (hours > 0) {
		ui.hourSpin->setValue(ui.hourSpin->value() + hours);
		ui.minuteSpin->setValue(value % 60);
	}
}

void EventTimeEditWidget::on_secondSpin_valueChanged(int value) {
	int mins = value / 60;
	if (mins > 0) {
		ui.minuteSpin->setValue(ui.minuteSpin->value() + mins);
		ui.secondSpin->setValue(value % 60);
	}
}

int EventTimeEditWidget::getTime() const {
//	cerr << "setTime" << endl;
	int time = ui.daySpin->value() * 24 * 3600 + ui.hourSpin->value() * 3600
			+ ui.minuteSpin->value() * 60 + ui.secondSpin->value();
	if (ui.backwardCheck->isChecked()) {
		time = -time;
	}
	return time;
}
