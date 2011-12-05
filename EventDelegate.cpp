/*
 * EventDelegate.cpp
 *
 *  Created on: Nov 9, 2011
 *      Author: vincent
 */

#include "EventDelegate.h"
#include <QApplication>

EventDelegate::EventDelegate(QObject *parent) :
		QStyledItemDelegate(parent) {
}

QWidget *EventDelegate::createEditor(QWidget *parent,
		const QStyleOptionViewItem &option, const QModelIndex &index) const {
	return QStyledItemDelegate::createEditor(parent, option, index);
}

void EventDelegate::setEditorData(QWidget *editor,
		const QModelIndex &index) const {
	QStyledItemDelegate::setEditorData(editor, index);
}

void EventDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const {
	QStyledItemDelegate::setModelData(editor, model, index);
}

void EventDelegate::updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const {
	QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

void EventDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const {
	if (index.column() == 3) {
		int offset = index.data().toInt();
		QApplication::style()->drawItemText(painter, option.rect,
				Qt::AlignLeft | Qt::AlignVCenter, QApplication::palette(), true,
				getOffsetString(offset));
	} else {
		QStyledItemDelegate::paint(painter, option, index);
	}
}

QString EventDelegate::getOffsetString(int secs) {
	QString resultString;
	if (secs < 0) {
		secs = -secs;
		resultString = "- ";
	} else {
		resultString = "+ ";
	}
	int days = secs / 24 / 3600;
	int hours = secs % (24 * 3600) / 3600;
	int mins = secs % (3600) / 60;
	secs = secs % 60;
	if (days > 0) {
		resultString += QString("%1 %2 ").arg(days).arg(
				days == 1 ? "day" : "days");
	}
	if (hours > 0) {
		resultString += QString("%1 %2 ").arg(hours).arg(
				hours == 1 ? "hour" : "hours");
	}
	if (mins > 0) {
		resultString += QString("%1 %2").arg(mins).arg(
				mins == 1 ? "min" : "mins");
	}
	if (mins > 0) {
		resultString += QString("%1 %2").arg(secs).arg(
				secs == 1 ? "sec" : "secs");
	}
	if (resultString.isEmpty()) {
		resultString = "0 sec";
	}
	return resultString;
}
