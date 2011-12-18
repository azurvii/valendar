/*
 * EventDelegate.cpp
 *
 *  Created on: Nov 9, 2011
 *      Author: vincent
 */

#include "EventDelegate.h"
#include <QApplication>
#include <iostream>
//#include <QtGui>
#include <QPainter>
#include "EventTimeEditWidget.h"

EventDelegate::EventDelegate(QObject *parent) :
		QStyledItemDelegate(parent) {
}

QWidget *EventDelegate::createEditor(QWidget *parent,
		const QStyleOptionViewItem &option, const QModelIndex &index) const {
	int col = index.column();
//	index.model()->headerData(col, Qt::Horizontal);
	EventTimeEditWidget *editor;
	switch (col) {
	case 3:
	case 4:
//		int offset = index.data().toInt();
		editor = new EventTimeEditWidget(col == 3 ? true : false, parent);
//		editor->setGeometry(option.rect.x(), option.rect.y(),
//				option.rect.width(), editor->height());
//		spinBox->setRange(col == 4 ? 0 : -999999999, 999999999);
//		spinBox->setSuffix("s");
//		QApplication::style()->drawItemText(painter, option.rect,
//				Qt::AlignLeft | Qt::AlignVCenter, QApplication::palette(), true,
//				getOffsetString(offset));
		return editor;
	default:
		return QStyledItemDelegate::createEditor(parent, option, index);
	}
}

void EventDelegate::setEditorData(QWidget *editor,
		const QModelIndex &index) const {
	int col = index.column();
	switch (col) {
	case 3:
	case 4:
		((EventTimeEditWidget*) editor)->setTime(index.data().toInt());
		break;
	default:
		QStyledItemDelegate::setEditorData(editor, index);
		break;
	}
}

void EventDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const {
	int col = index.column();
	switch (col) {
	case 3:
	case 4:
		model->setData(index, ((EventTimeEditWidget*) editor)->getTime());
		break;
	default:
		QStyledItemDelegate::setModelData(editor, model, index);
		break;
	}
}

void EventDelegate::updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const {
	int col = index.column();
	switch (col) {
	case 3:
	case 4:
		editor->setGeometry(option.rect.x(), option.rect.y(),
				option.rect.width(), editor->height());
		break;
	default:
		QStyledItemDelegate::updateEditorGeometry(editor, option, index);
		break;
	}
}

void EventDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const {
	int col = index.column();
	QString timeString;
//	index.model()->headerData(col, Qt::Horizontal);
	switch (col) {
	case 3: //
	case 4:
//		int offset = index.data().toInt();
		painter->setFont(QFont("Monospace"));
		timeString = getOffsetString(index.data().toInt());
		if (col == 4 && !timeString.startsWith("-")) {
			timeString = "+" + timeString;
		}
		QApplication::style()->drawItemText(painter, option.rect,
				Qt::AlignRight | Qt::AlignVCenter, QApplication::palette(),
				true, timeString);
		break;
	default:
		QStyledItemDelegate::paint(painter, option, index);
		break;
	}
}

QString EventDelegate::getOffsetString(int time) {
	QString resultString;
	if (time < 0) {
		time = -time;
		resultString = "-";
//	} else {
//		resultString = "+";
	}
	int days = time / 24 / 3600;
	int hours = (time % (24 * 3600)) / 3600;
	int mins = (time % (3600)) / 60;
	int secs = time % 60;
	bool needSpace = false;
	if (days > 0) {
		resultString += QString("%1d").arg(days);
		needSpace = true;
	}
	if (time % (24 * 3600) != 0 || needSpace == false) {
		resultString += QString(needSpace ? " %1:%2:%3" : "%1:%2:%3").arg(hours,
				2, 10, QChar('0')).arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10,
				QChar('0'));
	}
	return resultString;
}
