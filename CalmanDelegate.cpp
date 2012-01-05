/*
 * CalmanDelegate.cpp
 *
 *  Created on: Jan 4, 2012
 *      Author: vincent
 */

#include "CalmanDelegate.h"
#include <QApplication>
#include <QDateTime>

CalmanDelegate::CalmanDelegate(QObject *parent) :
		QStyledItemDelegate(parent) {
}

void CalmanDelegate::paint(QPainter *painter,
		const QStyleOptionViewItem &option, const QModelIndex &index) const {
	int col = index.column();
	QString timeString;
	QStyleOptionButton check_box_style_option;
	switch (col) {
	case 3: //
		QApplication::style()->drawItemText(painter, option.rect,
				Qt::AlignRight | Qt::AlignVCenter, QApplication::palette(),
				true, QDateTime::fromTime_t(index.data().toInt()).toString());
		break;
	default:
		QStyledItemDelegate::paint(painter, option, index);
		break;
	}
}
