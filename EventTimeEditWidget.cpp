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
	ui.directionCombo->setVisible(backward);
}

void EventTimeEditWidget::setTime(int secs) {
//	cerr << "setTime" << endl;
	if (secs < 0) {
		secs = -secs;
		ui.directionCombo->setCurrentIndex(1);
	} else {
		ui.directionCombo->setCurrentIndex(0);
	}
	ui.daySpin->setValue(secs / 24 / 3600);
	if (allday) {
		ui.hourSpin->setValue(0);
		ui.minuteSpin->setValue(0);
		ui.secondSpin->setValue(0);
	} else {
		ui.hourSpin->setValue((secs % (24 * 3600)) / 3600);
		ui.minuteSpin->setValue((secs % (3600)) / 60);
		ui.secondSpin->setValue(secs % 60);
	}
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
	int time;
	if (allday) {
		time = ui.daySpin->value() * 24 * 3600;
	} else {
		time = ui.daySpin->value() * 24 * 3600 + ui.hourSpin->value() * 3600
				+ ui.minuteSpin->value() * 60 + ui.secondSpin->value();
	}
	if (ui.directionCombo->currentIndex()) {
		time = -time;
	}
	return time;
}

void EventTimeEditWidget::setAllDayMode(bool allday) {
	this->allday = allday;
	if (allday) {
//		ui.daySpin->setValue(secs / 24 / 3600);
		ui.hourSpin->setValue(0);
		ui.minuteSpin->setValue(0);
		ui.secondSpin->setValue(0);
		ui.hourSpin->setVisible(false);
		ui.minuteSpin->setVisible(false);
		ui.secondSpin->setVisible(false);
	} else {
		ui.hourSpin->setVisible(true);
		ui.minuteSpin->setVisible(true);
		ui.secondSpin->setVisible(true);
	}
}

void EventTimeEditWidget::makeFocus() {
	ui.daySpin->setFocus();
}
