/*
 * EventDelegate.h
 *
 *  Created on: Nov 9, 2011
 *      Author: vincent
 */

#ifndef EVENTDELEGATE_H_
#define EVENTDELEGATE_H_

#include <QStyledItemDelegate>

class EventDelegate: public QStyledItemDelegate {
Q_OBJECT
public:
	EventDelegate(QObject *parent = 0);
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
			const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
			const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor,
			const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void paint(QPainter *painter, const QStyleOptionViewItem &option,
			const QModelIndex &index) const;

private:
	static QString getOffsetString(int secs);
};

#endif /* EVENTDELEGATE_H_ */
