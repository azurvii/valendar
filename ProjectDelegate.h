/*
 * ProjectDelegate.h
 *
 *  Created on: Nov 9, 2011
 *      Author: vincent
 */

#ifndef PROJECTDELEGATE_H_
#define PROJECTDELEGATE_H_

#include <QItemDelegate>

class ProjectDelegate: public QItemDelegate {
	Q_OBJECT
public:
	ProjectDelegate(QObject *parent = 0);
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
			const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
			const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor,
			const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif /* PROJECTDELEGATE_H_ */
