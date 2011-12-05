/*
 * ProjectDelegate.cpp
 *
 *  Created on: Nov 9, 2011
 *      Author: vincent
 */

#include "ProjectDelegate.h"

ProjectDelegate::ProjectDelegate(QObject *parent) :
		QItemDelegate(parent) {

}

QWidget *ProjectDelegate::createEditor(QWidget *parent,
		const QStyleOptionViewItem &option, const QModelIndex &index) const {
	return QItemDelegate::createEditor(parent, option, index);
}

void ProjectDelegate::setEditorData(QWidget *editor,
		const QModelIndex &index) const {
	QItemDelegate::setEditorData(editor, index);
}

void ProjectDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const {
	QItemDelegate::setModelData(editor, model, index);
}

void ProjectDelegate::updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const {
	QItemDelegate::updateEditorGeometry(editor, option, index);
}
