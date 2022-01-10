#pragma once

#include <A3DSDKIncludes.h>
#include <Qt3DCore>

inline QVector3D toQVector3D( A3DVector3dData const &v ) {
    return QVector3D( static_cast<float>(v.m_dX), static_cast<float>(v.m_dY), static_cast<float>(v.m_dZ) );
}

using EntityArray = QVector<A3DEntity*>;
Qt3DCore::QTransform *createTransform( EntityArray const &instance_path );
