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

#include "qdeclarativecirclemapitem_p.h"
#include "qdeclarativegeomapquickitem_p.h"
#include "projection_p.h"
#include <cmath>
#include <QPen>
#include <QPainter>

QT_BEGIN_NAMESPACE

//TODO: make export ??

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const qreal qgeocoordinate_EARTH_MEAN_RADIUS = 6371.0072;

inline static qreal qgeocoordinate_degToRad(qreal deg)
{
    return deg * M_PI / 180;
}
inline static qreal qgeocoordinate_radToDeg(qreal rad)
{
    return rad * 180 / M_PI;
}

static QPolygonF createPolygon(const Map& map, const QList<QGeoCoordinate> &path, qreal& w, qreal& h)
{
    QPolygonF points;

    qreal minX, maxX, minY, maxY;
    //TODO: dateline handling

    for (int i = 0; i < path.size(); ++i) {

        const QGeoCoordinate &coord = path.at(i);

        if (!coord.isValid())
            continue;

        QPointF point = map.coordinateToScreenPosition(coord, false);

        if (i == 0) {
            minX = point.x();
            maxX = point.x();
            minY = point.y();
            maxY = point.y();
        } else {
            minX = qMin(point.x(), minX);
            maxX = qMax(point.x(), maxX);
            minY = qMin(point.y(), minY);
            maxY = qMax(point.y(), maxY);
        }
        points.append(point);
    }

    points.translate(-minX, -minY);

    w = maxX - minX;
    h = maxY - minY;

    return points;
}

QDeclarativeCircleMapItem::QDeclarativeCircleMapItem(QQuickItem *parent):
        QDeclarativeGeoMapItemBase(parent),
    center_(0),
    circleItem_(new CircleMapPaintedItem(this)),
    inUpdate_(false),
    zoomLevel_(0.0),
    dragActive_(false)
{
    circleItem_->setParentItem(this);
}

QDeclarativeCircleMapItem::~QDeclarativeCircleMapItem()
{
}

void QDeclarativeCircleMapItem::setCenter(QDeclarativeCoordinate *center)
{
    if (center_ == center)
        return;
    if (center_)
        center_->disconnect(this);
    center_ = center;
    if (!center_) {
        circleItem_->setCenter(QGeoCoordinate());
    } else {
        circleItem_->setCenter(center_->coordinate());
        connect(center_, SIGNAL(latitudeChanged(double)), this,
                SLOT(handleCenterCoordinateChanged()));
        connect(center_, SIGNAL(longitudeChanged(double)), this,
                SLOT(handleCenterCoordinateChanged()));
        connect(center_, SIGNAL(altitudeChanged(double)), this,
                SLOT(handleCenterCoordinateChanged()));
    }
    emit centerChanged(center_);
}

QDeclarativeCoordinate* QDeclarativeCircleMapItem::center()
{
    return center_;
}

void QDeclarativeCircleMapItem::handleCenterCoordinateChanged()
{
    circleItem_->setCenter(center_->coordinate());
}

void QDeclarativeCircleMapItem::handleCameraDataChanged(const CameraData& cameraData)
{
    circleItem_->setZoomLevel(cameraData.zoomFactor());
    update();
}

void QDeclarativeCircleMapItem::setColor(const QColor &color)
{
    if (color_ == color)
        return;
    color_ = color;
    QBrush m_brush(color);
    circleItem_->setBrush(m_brush);
    emit colorChanged(color_);
}

QColor QDeclarativeCircleMapItem::color() const
{
    return color_;
}

void QDeclarativeCircleMapItem::setRadius(qreal radius)
{
    if (circleItem_->radius() == radius)
        return;
    circleItem_->setRadius(radius);
    emit radiusChanged(radius);
}

qreal QDeclarativeCircleMapItem::radius() const
{
    return circleItem_->radius();
}

void QDeclarativeCircleMapItem::update()
{
    if (inUpdate_ || !map())
        return;

    inUpdate_ = true;
    QPointF topLeft = map()->coordinateToScreenPosition(center()->coordinate(), false) - QPointF(width(),height()) / 2;
    if ((topLeft.x() > quickMap()->width())
            || (topLeft.x() + width() < 0)
            || (topLeft.y() + height() < 0)
            || (topLeft.y() > quickMap()->height())) {
        setPos(map()->coordinateToScreenPosition(QGeoCoordinate()));
    } else {
        setWidth(circleItem_->width());
        setHeight(circleItem_->height());
        setPos(topLeft);
    }
    // optimize this check/calls, need to be done only when map changes:
    if (!circleItem_->map()) {
        circleItem_->setMap(map());
        connect(map(), SIGNAL(cameraDataChanged(CameraData)), this, SLOT(handleCameraDataChanged(CameraData)));
    }
    inUpdate_ = false;
}

void QDeclarativeCircleMapItem::dragEnded()
{
    if (!dragActive_)
        return;
    dragActive_ = false;
    QPointF newPoint = QPointF(x(),y()) + QPointF(width(), height()) / 2;
    QGeoCoordinate newCoordinate = map()->screenPositionToCoordinate(newPoint, false);
    if (newCoordinate.isValid()) {
        internalCoordinate_.setCoordinate(newCoordinate);
        setCenter(&internalCoordinate_);
    }
}

void QDeclarativeCircleMapItem::dragStarted()
{
    dragActive_ = true;
}

bool QDeclarativeCircleMapItem::contains(QPointF point)
{
    return circleItem_->contains(point);
}

void QDeclarativeCircleMapItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    // TODO - if X and Y of the wrapper item are changed, currently
    // the item moves, but returns to old position when map camera changes
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

//////////////////////////////////////////////////////////////////////

CircleMapPaintedItem::CircleMapPaintedItem(QQuickItem *parent):
        QQuickPaintedItem(parent),
        map_(0),
        zoomLevel_(-1),
        initialized_(false)
{
    setAntialiasing(true);
    connect(this, SIGNAL(xChanged()), this, SLOT(update()));
    connect(this, SIGNAL(yChanged()), this, SLOT(update()));
}

CircleMapPaintedItem::~CircleMapPaintedItem() {}

void CircleMapPaintedItem::setMap(Map* map)
{
    if (map_ == map)
        return;
    map_ = map;
}

Map* CircleMapPaintedItem::map()
{
    return map_;
}

void CircleMapPaintedItem::setZoomLevel(qreal zoomLevel)
{
    if (zoomLevel_ == zoomLevel)
        return;

    zoomLevel_ = zoomLevel;
    updateGeometry();
}

qreal CircleMapPaintedItem::zoomLevel() const
{
    return zoomLevel_;
}

void CircleMapPaintedItem::paint(QPainter *painter)
{
    if (!initialized_)
        return;

    painter->setPen(pen_);
    painter->setBrush(brush_);
    painter->drawConvexPolygon(polygon_);
}

void CircleMapPaintedItem::updateGeometry()
{
    initialized_ = false;
    if (!map_)
        return;

    if (!centerCoord_.isValid() || radius_ <= 0)
        return;

    if (zoomLevel_ == -1)
        return;

    QPointF center = map_->coordinateToScreenPosition(centerCoord_, false);

    qreal w = 0;
    qreal h = 0;
    //TODO: execute only for radius or center changes
    path_.clear();
    calcualtePeripheralPoints(path_, centerCoord_, radius_, 125);
    //TODO: optimize essential part
    polygon_ = createPolygon(*map_, path_, w, h);

    setWidth(w);
    setHeight(h);
    setContentsSize(QSize(w, h));

    initialized_ = true;
    update();
}

void CircleMapPaintedItem::calcualtePeripheralPoints(QList<QGeoCoordinate>& path, const QGeoCoordinate& center, qreal distance, int steps) const
{
    // get angular distance in radians
    distance = distance / (qgeocoordinate_EARTH_MEAN_RADIUS * 1000);

    // We are using horizontal system, we have radius (distance)
    // projected onto Celestial sphere.
    // This way we know the altitude in horizontal system => h = 90 - r;
    // We can now "spin" around with azimuth as a step to get all the points from
    // peripheral of the given circle.
    // To get geographical position we need to change from horizontal system
    // to equatorial system.

    // get location
    qreal lat = qgeocoordinate_degToRad(center.latitude());
    qreal lon = qgeocoordinate_degToRad(center.longitude());

    // precalculate
    qreal cos_h = sin(distance);
    qreal sin_h = cos(distance);
    qreal cos_phi = cos(lat), sin_phi = sin(lat);
    qreal sin_phi_x_sin_h = sin_phi * sin_h;
    qreal cos_phi_x_cos_h = cos_phi * cos_h;
    qreal sin_phi_x_cos_h = sin_phi * cos_h;
    qreal cos_phi_x_sin_h = cos_phi * sin_h;

    for (int i = 0; i < steps; ++i) {

        qreal a = 2 * M_PI * i / steps;
        qreal sin_delta = sin_phi_x_sin_h - cos_phi_x_cos_h * cos(a);
        qreal cos_delta_x_cos_tau = cos_phi_x_sin_h + sin_phi_x_cos_h * cos(a);
        qreal cos_delta_x_sin_tau = -sin(a) * cos_h;
        // get the hour angle (use Cartesian to polar conversion)
        qreal tau = atan2(cos_delta_x_sin_tau, cos_delta_x_cos_tau);
        qreal cos_delta = sqrt(cos_delta_x_sin_tau
                               * cos_delta_x_sin_tau + cos_delta_x_cos_tau
                               * cos_delta_x_cos_tau);
        // get declination ( use Cartesian to polar conversion )
        qreal delta = atan2(sin_delta, cos_delta);
        // get right ascension from tau , use a greenwich star time of 0
        qreal alpha = lon - tau;
        qreal lat2 = qgeocoordinate_radToDeg(delta);
        qreal lon2 = qgeocoordinate_radToDeg(alpha);
        if (lon2 < -180.0) {
            lon2 += 360.0;
        } else if (lon2 > 180.0) {
            lon2 -= 360.0;
        }
        path << QGeoCoordinate(lat2, lon2, 0.0f);
    }
}


void CircleMapPaintedItem::setBrush(const QBrush &brush)
{
    brush_ = brush;
}

QBrush CircleMapPaintedItem::brush() const
{
    return brush_;
}

void CircleMapPaintedItem::setPen(const QPen &pen)
{
    pen_ = pen;
}

QPen CircleMapPaintedItem::pen() const
{
    return pen_;
}

void CircleMapPaintedItem::setCenter(const QGeoCoordinate &center)
{
    if (centerCoord_ == center)
        return;
    centerCoord_ = center;
    updateGeometry();
}

const QGeoCoordinate& CircleMapPaintedItem::center() const
{
    return centerCoord_;
}

bool CircleMapPaintedItem::contains(QPointF point)
{
    return polygon_.containsPoint(point, Qt::OddEvenFill);
}

void CircleMapPaintedItem::setRadius(qreal radius)
{
    if (radius_ == radius)
        return;

    radius_ = radius;
    updateGeometry();
}

qreal CircleMapPaintedItem::radius() const
{
    return radius_;
}

QT_END_NAMESPACE
