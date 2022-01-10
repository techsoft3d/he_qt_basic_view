#include "Scene.h"
#include "ForEachRepresentationItem.h"
#include "Material.h"
#include "Transform.h"
#include "Mesh.h"
#include "CascadedAttributes.h"

Qt3DCore::QEntity *createScene( A3DAsmModelFile *model_file ) {
    Q_UNUSED(model_file)
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;
    return rootEntity;
}
