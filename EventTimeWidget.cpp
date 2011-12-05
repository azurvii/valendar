/*
 * EventTimeWidget.cpp
 *
 *  Created on: Dec 5, 2011
 *      Author: vincent
 */

#include "EventTimeWidget.h"

EventTimeWidget::EventTimeWidget(QWidget *parent) :
		QWidget(parent) {
	ui.setupUi(this);
}

//void EventTimeWidget::on_daySpin_valueChanged(int value) {
//	if (value > 1 || value < -1) {
//		ui.daySpin->setSuffix("days");
//	} else {
//		ui.daySpin->setSuffix("day");
//	}
//}
//
//void EventTimeWidget::on_hourSpin_valueChanged(int value) {
//	if (value > 1 || value < -1) {
//		ui.hourSpin->setSuffix("hours");
//	} else {
//		ui.hourSpin->setSuffix("hour");
//	}
//}
