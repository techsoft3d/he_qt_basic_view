#pragma once

#include <QtCore>
#include <A3DSDKIncludes.h>
#include <functional>

using EntityArray = QVector<A3DEntity*>;

void forEach_RepresentationItem( EntityArray const &path, std::function<void(EntityArray const&)> const &fcn );