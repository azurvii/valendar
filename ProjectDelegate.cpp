/*
 * ProjectDelegate.cpp
 *
 *  Created on: Nov 9, 2011
 *      Author: vincent
 */

#include "ProjectDelegate.h"
#include <QPainter>
#include <QApplication>
#include <QDateTime>
#include <iostream>

ProjectDelegate::ProjectDelegate(QObject *parent) :
		QStyledItemDelegate(parent) {
}

QWidget *ProjectDelegate::createEditor(QWidget *parent,
		const QStyleOptionViewItem &option, const QModelIndex &index) const {
	int col = index.column();
	switch (col) {
//	case 2:
	case 3:
//	case 4:
//		int offset = index.data().toInt();
//		editor = new EventTimeEditWidget(col == 3 ? true : false, parent);
//		editor->setGeometry(option.rect.x(), option.rect.y(),
//				option.rect.width(), editor->height());
//		spinBox->setRange(col == 4 ? 0 : -999999999, 999999999);
//		spinBox->setSuffix("s");
//		QApplication::style()->drawItemText(painter, option.rect,
//				Qt::AlignLeft | Qt::AlignVCenter, QApplication::palette(), true,
//				getOffsetString(offset));
//		std::cout << index.data().toString().toStdString() << std::endl;
		return 0;
	default:
		return QStyledItemDelegate::createEditor(parent, option, index);
	}
}

void ProjectDelegate::setEditorData(QWidget *editor,
		const QModelIndex &index) const {
	QStyledItemDelegate::setEditorData(editor, index);
}

void ProjectDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const {
	QStyledItemDelegate::setModelData(editor, model, index);
}

void ProjectDelegate::updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const {
	QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

void ProjectDelegate::paint(QPainter *painter,
		const QStyleOptionViewItem &option, const QModelIndex &index) const {
	int col = index.column();
//	QString timeString;
//	index.model()->headerData(col, Qt::Horizontal);
	switch (col) {
	case 3: //
//	case 4:
//		int offset = index.data().toInt();
		painter->setFont(QFont("Monospace"));
//		if (col == 4 && !timeString.startsWith("-")) {
//			timeString = "+" + timeString;
//		}
		QApplication::style()->drawItemText(
				painter,
				option.rect,
				Qt::AlignLeft | Qt::AlignVCenter,
				QApplication::palette(),
				true,
				QDateTime::fromTime_t(index.data().toInt()).toString(
						"yyyy-MM-dd hh:mm:ss"));
		break;
	default:
		QStyledItemDelegate::paint(painter, option, index);
		break;
	}
}
