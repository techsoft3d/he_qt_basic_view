#include <Qt3DExtras>
#include "Material.h"


Qt3DCore::QComponent *createMaterial( A3DGraphStyleData const &style_data ) {
    Q_UNUSED(style_data);
    auto material = new Qt3DExtras::QDiffuseSpecularMaterial();
    material->setDiffuse(QColor("red"));
    return material;
}