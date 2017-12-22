/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPLACESEARCHREQUEST_P_H
#define QPLACESEARCHREQUEST_P_H

#include "qplacesearchrequest.h"
#include "qgeocoordinate.h"
#include "qgeoshape.h"

#include <QtCore/QSharedData>
#include <QtCore/QList>
#include <QtLocation/private/qlocationglobal_p.h>
#include <QtCore/QVariant>

QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT QPlaceSearchRequestPrivate : public QSharedData
{
public:
    QPlaceSearchRequestPrivate();
    QPlaceSearchRequestPrivate(const QPlaceSearchRequestPrivate &other);
    ~QPlaceSearchRequestPrivate();

    QPlaceSearchRequestPrivate &operator=(const QPlaceSearchRequestPrivate &other);
    bool operator==(const QPlaceSearchRequestPrivate &other) const;

    void clear();
    static const QPlaceSearchRequestPrivate *get(const QPlaceSearchRequest &request);
    static QPlaceSearchRequestPrivate *get(QPlaceSearchRequest &request);

    QString searchTerm;
    QList<QPlaceCategory> categories;
    QGeoShape searchArea;
    QString recommendationId;
    QLocation::VisibilityScope visibilityScope;
    QPlaceSearchRequest::RelevanceHint relevanceHint;
    int limit;
    QVariant searchContext;
    bool related = false;
    int page = 0;
};

QT_END_NAMESPACE

#endif // QPLACESEARCHREQUEST_P_H
