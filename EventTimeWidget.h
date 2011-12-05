/*
 * EventTimeWidget.h
 *
 *  Created on: Dec 5, 2011
 *      Author: vincent
 */

#ifndef EVENTTIMEWIDGET_H_
#define EVENTTIMEWIDGET_H_

#include "ui_EventTimeWidget.h"

class EventTimeWidget: QWidget {
Q_OBJECT
public:
	EventTimeWidget(QWidget *parent = 0);

//public slots:
//	void on_daySpin_valueChanged(int value);
//	void on_hourSpin_valueChanged(int value);

private:
	Ui::EventTimeWidget ui;
};

#endif /* EVENTTIMEWIDGET_H_ */
