#include "ForEachRepresentationItem.h"

namespace {
    A3DAsmPartDefinition *getPart( A3DAsmProductOccurrence *po ) {
        if( nullptr == po ) {
            return nullptr;
        }

        A3DAsmProductOccurrenceData pod;
        A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, pod);
        if(A3D_SUCCESS != A3DAsmProductOccurrenceGet( po, &pod ) ) {
            return nullptr;
        }
        auto part =  pod.m_pPart ? pod.m_pPart : getPart( pod.m_pPrototype );
        A3DAsmProductOccurrenceGet( nullptr, &pod );
        return part;
    }
}

void forEach_RepresentationItem( EntityArray const &path, std::function<void(EntityArray const&)> const &fcn ) {
    auto const ntt = path.back();
    auto type = kA3DTypeUnknown;
    if(A3D_SUCCESS != A3DEntityGetType( ntt, &type) ) {
        return;
    }

    EntityArray children;
    if(kA3DTypeAsmModelFile == type) {
        A3DAsmModelFileData mfd;
        A3D_INITIALIZE_DATA(A3DAsmModelFileData, mfd);
        if(A3D_SUCCESS != A3DAsmModelFileGet( ntt, &mfd ) ) {
            return;
        }
        children = EntityArray( mfd.m_ppPOccurrences, mfd.m_ppPOccurrences + mfd.m_uiPOccurrencesSize );
        A3DAsmModelFileGet( nullptr, &mfd );
    } else if( kA3DTypeAsmProductOccurrence == type ) {
        A3DAsmProductOccurrenceData pod;
        A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, pod);
        if(A3D_SUCCESS != A3DAsmProductOccurrenceGet( ntt, &pod ) ) {
            return;
        }
        children = EntityArray( pod.m_ppPOccurrences, pod.m_ppPOccurrences + pod.m_uiPOccurrencesSize );
        if( auto part = pod.m_pPart ? pod.m_pPart : getPart( pod.m_pPrototype ) ) {
            children.insert( children.begin(), part );

        }
        A3DAsmProductOccurrenceGet( nullptr, &pod );
    } else if( kA3DTypeAsmPartDefinition == type ) {
        A3DAsmPartDefinitionData pdd;
        A3D_INITIALIZE_DATA(A3DAsmPartDefinitionData, pdd);
        if(A3D_SUCCESS != A3DAsmPartDefinitionGet( ntt, &pdd ) ) {
            return;
        }
        children = EntityArray( pdd.m_ppRepItems, pdd.m_ppRepItems + pdd.m_uiRepItemsSize );
        A3DAsmPartDefinitionGet( nullptr, &pdd );
    } else {
        if( kA3DTypeRiSet == type ) {
            A3DRiSetData risd;
            A3D_INITIALIZE_DATA(A3DRiSetData, risd);
            if(A3D_SUCCESS != A3DRiSetGet( ntt, &risd ) ) {
                return;
            }
            children = EntityArray( risd.m_ppRepItems, risd.m_ppRepItems + risd.m_uiRepItemsSize );
            A3DRiSetGet( nullptr, &risd );
        } else {
            fcn( path );
        }
    }

    for( auto child : children ) {
        auto child_path = path;
        child_path.push_back( child );
        forEach_RepresentationItem( child_path, fcn );
    }
}
