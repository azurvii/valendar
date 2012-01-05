/*
 * CalmanDelegate.h
 *
 *  Created on: Jan 4, 2012
 *      Author: vincent
 */

#ifndef CALMANDELEGATE_H_
#define CALMANDELEGATE_H_

#include <QStyledItemDelegate>

class CalmanDelegate: public QStyledItemDelegate {
Q_OBJECT
public:
	CalmanDelegate(QObject *parent = 0);
	void paint(QPainter *painter, const QStyleOptionViewItem &option,
			const QModelIndex &index) const;
};

#endif /* CALMANDELEGATE_H_ */
