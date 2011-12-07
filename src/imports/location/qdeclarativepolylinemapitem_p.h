/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEPOLYLINEMAPITEM
#define QDECLARATIVEPOLYLINEMAPITEM

#include "qdeclarativegeomapitembase_p.h"
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeomap_p.h"
#include <QPen>
#include <QBrush>

QT_BEGIN_NAMESPACE

class PolylineMapPaintedItem;

class QDeclarativePolylineMapItem : public QDeclarativeGeoMapItemBase
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeCoordinate> path READ declarativePath NOTIFY pathChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    QDeclarativePolylineMapItem(QQuickItem *parent = 0);
    ~QDeclarativePolylineMapItem();

    virtual void componentComplete();

    Q_INVOKABLE void addCoordinate(QDeclarativeCoordinate* coordinate);
    Q_INVOKABLE void removeCoordinate(QDeclarativeCoordinate* coordinate);

    QDeclarativeListProperty<QDeclarativeCoordinate> declarativePath();

    QColor color() const;
    void setColor(const QColor &color);

    void dragStarted();
    bool contains(QPointF point);

Q_SIGNALS:
    void pathChanged();
    void colorChanged(const QColor &color);

protected Q_SLOTS:
    void update();
    // from qquickitem
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

private Q_SLOTS:
    // map size changed
    void handleCameraDataChanged(const CameraData& cameraData);

private:
    static void path_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, QDeclarativeCoordinate *coordinate);
    static int path_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop);
    static QDeclarativeCoordinate* path_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, int index);
    static void path_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop);
    void pathPropertyChanged();

private:
    PolylineMapPaintedItem *polylineItem_;
    QList<QDeclarativeCoordinate*> path_;
    QColor color_;
    bool initialized_;
    bool inUpdate_;
    bool zoomLevel_;
};

//////////////////////////////////////////////////////////////////////

class PolylineMapPaintedItem : public QQuickPaintedItem
{
    Q_OBJECT

public:
    PolylineMapPaintedItem(QQuickItem *parent = 0);
    ~PolylineMapPaintedItem();

    void setMap(Map* map);
    Map* map();

    void setZoomLevel(qreal zoomLevel);
    qreal zoomLevel() const;

    QList<QGeoCoordinate> path() const;
    void setPath(const QList<QGeoCoordinate>& path);

    void paint(QPainter *painter);

    QPen pen() const;
    void setPen(const QPen &pen);

    QBrush brush() const;
    void setBrush(const QBrush &brush);

    bool contains(QPointF point);

    QGeoCoordinate quickItemCoordinate() const;
    QPointF quickItemAnchorPoint() const;

private:
    void updateGeometry();
    Map *map_;
    qreal zoomLevel_;
    QPen pen_;
    QBrush brush_;
    QGeoCoordinate quickItemCoordinate_;
    QPointF quickItemAnchorPoint_;
    QList<QGeoCoordinate> coordPath_;
    QPainterPath path_;
    bool initialized_;
};


QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativePolylineMapItem));

#endif /* QDECLARATIVEPOLYLINEMAPITEM_H_ */
