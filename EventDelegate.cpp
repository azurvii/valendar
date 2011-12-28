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
#include <QMouseEvent>
#include "GalendarForm.h"

EventDelegate::EventDelegate(QObject *parent) :
		QStyledItemDelegate(parent) {
}

QWidget *EventDelegate::createEditor(QWidget *parent,
		const QStyleOptionViewItem &option, const QModelIndex &index) const {
//	std::cout << "createEditor" << std::endl;
	int col = index.column();
//	index.model()->headerData(col, Qt::Horizontal);
	EventTimeEditWidget *editor;
	switch (col) {
	case 3:
	case 4:
//		int offset = index.data().toInt();
		editor = new EventTimeEditWidget(col == 3 ? true : false, parent);
		editor->setAllDayMode(index.sibling(index.row(), 5).data().toBool());
//		editor->setGeometry(option.rect.x(), option.rect.y(),
//				option.rect.width(), editor->height());
//		spinBox->setRange(col == 4 ? 0 : -999999999, 999999999);
//		spinBox->setSuffix("s");
//		QApplication::style()->drawItemText(painter, option.rect,
//				Qt::AlignLeft | Qt::AlignVCenter, QApplication::palette(), true,
//				getOffsetString(offset));
		return editor;
//	case 5:
//		return new QCheckBox(parent);
	default:
		return QStyledItemDelegate::createEditor(parent, option, index);
	}
}

void EventDelegate::setEditorData(QWidget *editor,
		const QModelIndex &index) const {
//	std::cout << "setEditorData" << std::endl;
	int col = index.column();
	switch (col) {
	case 3:
	case 4:
		((EventTimeEditWidget *) editor)->setTime(index.data().toInt());
		break;
//	case 5:
//		((QCheckBox *) editor)->setChecked(index.data().toInt());
//		break;
	default:
		QStyledItemDelegate::setEditorData(editor, index);
		break;
	}
}

void EventDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const {
//	std::cout << "setModelData" << std::endl;
	int col = index.column();
	switch (col) {
	case 3:
	case 4:
		model->setData(index, ((EventTimeEditWidget*) editor)->getTime());
		break;
//	case 5:
//		model->setData(index, int(((QCheckBox *) editor)->isChecked()));
//		break;
	default:
		QStyledItemDelegate::setModelData(editor, model, index);
		break;
	}
}

void EventDelegate::updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const {
//	std::cout << "updateEditorGeometry" << std::endl;
	int col = index.column();
	switch (col) {
	case 3:
	case 4:
		editor->adjustSize();
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
//	std::cout << "paint" << std::endl;
	int col = index.column();
	QString timeString;
	QStyleOptionButton check_box_style_option;
//	index.model()->headerData(col, Qt::Horizontal);
	switch (col) {
	case 3: //
	case 4:
//		int offset = index.data().toInt();
		painter->setFont(QFont("Monospace"));
		timeString = getOffsetString(index.data().toInt(),
				index.sibling(index.row(), 5).data().toBool());
//		if (col == 4 && !timeString.startsWith("-")) {
//			timeString = "+" + timeString;
//		}
		QApplication::style()->drawItemText(painter, option.rect,
				Qt::AlignRight | Qt::AlignVCenter, QApplication::palette(),
				true, timeString);
		break;
	case 5:
		check_box_style_option.state |= QStyle::State_Enabled;
		check_box_style_option.state |=
				index.data().toBool() ? QStyle::State_On : QStyle::State_Off;
		check_box_style_option.rect = option.rect;
		QApplication::style()->drawControl(QStyle::CE_CheckBox,
				&check_box_style_option, painter);
		break;
	default:
		QStyledItemDelegate::paint(painter, option, index);
		break;
	}
}

QString EventDelegate::getOffsetString(int time, bool day) {
	QString resultString;
	if (time < 0) {
		time = -time;
		resultString = "-";
//	} else {
//		resultString = "+";
	} else if (time == 0) {
		resultString += day ? tr("0d") : tr("00:00:00");
		return resultString;
	}
	int days = time / 24 / 3600;
	int hours = (time % (24 * 3600)) / 3600;
	int mins = (time % (3600)) / 60;
	int secs = time % 60;
	bool needSpace = false;
	if (days > 0 /*&& (hours > 0 || mins > 0 || secs > 0)*/) {
		resultString += QString("%1d").arg(days);
		needSpace = true;
	}
	if (time % (24 * 3600) > 0) {
		resultString += (needSpace ? " " : "")
				+ tr("%1:%2:%3").arg(hours, 2, 10, QChar('0')).arg(mins, 2, 10,
						QChar('0')).arg(secs, 2, 10, QChar('0'));
	}
	return resultString;
}

bool EventDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
		const QStyleOptionViewItem &option, const QModelIndex &index) {
	if (index.column() != 5) {
		return QStyledItemDelegate::editorEvent(event, model, option, index);
	}
//	std::cout << "editorEvent" << std::endl;
	if ((event->type() == QEvent::MouseButtonRelease)
			|| (event->type() == QEvent::MouseButtonDblClick)) {
		QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);
		if (mouse_event->button() != Qt::LeftButton
				|| !option.rect.contains(mouse_event->pos())) {
			return false;
		}
		if (event->type() == QEvent::MouseButtonDblClick) {
			return true;
		}
	} else if (event->type() == QEvent::KeyPress) {
		if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space
				&& static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select) {
			return false;
		}
	} else {
		return false;
	}

	bool checked = index.model()->data(index, Qt::DisplayRole).toBool();
	bool returnval = model->setData(index, !checked, Qt::EditRole);
	if (!checked) {
		QModelIndex idx3 = index.sibling(index.row(), 3);
		int data3 = idx3.data().toInt();
		QModelIndex idx4 = index.sibling(index.row(), 4);
		int data4 = idx4.data().toInt();
		model->setData(idx3, data3 - data3 % (24 * 3600));
		model->setData(idx4, data4 - data4 % (24 * 3600));
	}
	return returnval;
}

