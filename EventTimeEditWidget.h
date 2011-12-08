/*
 * EventTimeEditWidget.h
 *
 *  Created on: Dec 8, 2011
 *      Author: vincent
 */

#ifndef EVENTTIMEEDITWIDGET_H_
#define EVENTTIMEEDITWIDGET_H_

#include "ui_EventTimeEditWidget.h"

class EventTimeEditWidget: public QWidget {
Q_OBJECT

public:
	EventTimeEditWidget(bool backward = true, QWidget *parent = 0);
	void setTime(int secs);
	int getTime() const;

public slots:
	void on_hourSpin_valueChanged(int value);
	void on_minuteSpin_valueChanged(int value);
	void on_secondSpin_valueChanged(int value);

private:
	Ui::EventTimeEditWidget ui;
};

#endif /* EVENTTIMEEDITWIDGET_H_ */
