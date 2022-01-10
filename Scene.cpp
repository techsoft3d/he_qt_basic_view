#include "Scene.h"
#include "ForEachRepresentationItem.h"
#include "Material.h"
#include "Transform.h"
#include "Mesh.h"
#include "CascadedAttributes.h"

Qt3DCore::QEntity *createScene( A3DAsmModelFile *model_file ) {
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;

    A3DRWParamsTessellationData tess_params;
    A3D_INITIALIZE_DATA( A3DRWParamsTessellationData, tess_params );
    tess_params.m_eTessellationLevelOfDetail = kA3DTessLODMedium;

    forEach_RepresentationItem( { model_file }, [&](EntityArray const &path) {
        CascadedAttributes ca( path );
        if( ca->m_bRemoved || !ca->m_bShow ) {
            return;
        }

        A3DRiRepresentationItem *ri = path.back();
        A3DRiRepresentationItemComputeTessellation( ri, &tess_params);
        
        A3DRiRepresentationItemData rid;
        A3D_INITIALIZE_DATA( A3DRiRepresentationItemData, rid );
        if( A3D_SUCCESS != A3DRiRepresentationItemGet( ri, &rid ) ) {
            return;
        }

        auto tess_base = rid.m_pTessBase;
        if( auto mesh = createMesh( tess_base ) ) {
            auto node = new Qt3DCore::QEntity( rootEntity );
            node->addComponent( mesh );
            if( auto material = createMaterial( ca->m_sStyle ) ) {
                node->addComponent( material );
            }
            if( auto transform = createTransform( path ) ) {
                node->addComponent( transform );
            }
        }

        A3DRiRepresentationItemGet( nullptr, &rid );
    });
    return rootEntity;
}
