/*
 * EventDelegate.cpp
 *
 *  Created on: Nov 9, 2011
 *      Author: vincent
 */

#include "EventDelegate.h"
#include <QApplication>
#include <iostream>
#include <QtGui>

EventDelegate::EventDelegate(QObject *parent) :
		QStyledItemDelegate(parent) {
}

QWidget *EventDelegate::createEditor(QWidget *parent,
		const QStyleOptionViewItem &option, const QModelIndex &index) const {
	int col = index.column();
	QSpinBox *spinBox;
	switch (col) {
	case 3:
	case 4:
//		int offset = index.data().toInt();
		spinBox = new QSpinBox(parent);
		spinBox->setRange(col == 4 ? 0 : -999999999, 999999999);
		spinBox->setSuffix("s");
//		QApplication::style()->drawItemText(painter, option.rect,
//				Qt::AlignLeft | Qt::AlignVCenter, QApplication::palette(), true,
//				getOffsetString(offset));
		return spinBox;
	default:
		return QStyledItemDelegate::createEditor(parent, option, index);
	}
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
	int col = index.column();
	switch (col) {
	case 3:
	case 4:
//		int offset = index.data().toInt();
		QApplication::style()->drawItemText(painter, option.rect,
				Qt::AlignLeft | Qt::AlignVCenter, QApplication::palette(), true,
				getOffsetString(index.data().toInt()));
		break;
	default:
		QStyledItemDelegate::paint(painter, option, index);
		break;
	}
}

QString EventDelegate::getOffsetString(int secs) {
	QString resultString;
	if (secs < 0) {
		secs = -secs;
		resultString = "-";
	} else {
		resultString = "+";
	}
	int days = secs / 24 / 3600;
	int hours = (secs % (24 * 3600)) / 3600;
	int mins = (secs % (3600)) / 60;
	secs = secs % 60;
	if (days > 0) {
		resultString += QString("%1d ").arg(days);
	}
	resultString += QString("%1:%2:%3").arg(hours, 2, 10, QChar('0')).arg(mins,
			2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
	return resultString;
}
