#include <Qt3DRender>
#include "Mesh.h"

using namespace Qt3DCore;

Qt3DCore::QComponent *createMesh( A3DTessBase *tess_base ) {
    if( nullptr == tess_base ) {
        return nullptr;
    }

    A3DEEntityType tess_type;
    if(A3D_SUCCESS != A3DEntityGetType( tess_base, &tess_type ) ) {
        return nullptr;
    }

    if( tess_type != kA3DTypeTess3D ) {
        return nullptr;
    }

    A3DTessBaseData tbd;
    A3D_INITIALIZE_DATA(A3DTessBaseData, tbd);
    if( A3D_SUCCESS != A3DTessBaseGet( tess_base, &tbd ) ) {
        return nullptr;
    }

    auto const coords = tbd.m_pdCoords;
    auto const n_coords = tbd.m_uiCoordSize;
    A3DTess3DData t3dd;
    A3D_INITIALIZE_DATA( A3DTess3DData, t3dd );
    if( A3D_SUCCESS != A3DTess3DGet( tess_base, &t3dd ) ) {
        A3DTessBaseGet( nullptr, &tbd );
        return nullptr;
    }

    auto const normals = t3dd.m_pdNormals;
    auto const n_normals = t3dd.m_uiNormalSize;

    QVector<quint32> q_indices;
    QByteArray bufferBytes;
    quint32 const stride = sizeof(float) * 6; // 3 for vertex + 3 for normal
    for( auto tess_face_idx = 0u; tess_face_idx < t3dd.m_uiFaceTessSize; ++tess_face_idx ) {
        auto const &d = t3dd.m_psFaceTessData[tess_face_idx];
        auto sz_tri_idx = 0u;
        auto ti_index = d.m_uiStartTriangulated;
        if(kA3DTessFaceDataTriangle & d.m_usUsedEntitiesFlags) {
            auto const num_tris = d.m_puiSizesTriangulated[sz_tri_idx++];
            auto const pt_count = num_tris * 3; // 3 pts per triangle
            auto const old_sz = bufferBytes.size();
            bufferBytes.resize(bufferBytes.size() + stride * pt_count);
            auto fptr = reinterpret_cast<float*>(bufferBytes.data() + old_sz);
            for(auto tri = 0u; tri < num_tris; tri++) {
                for(auto vert = 0u; vert < 3u; vert++) {
                    auto const &normal_index = t3dd.m_puiTriangulatedIndexes[ti_index++];
                    auto const &coord_index  = t3dd.m_puiTriangulatedIndexes[ti_index++];
                    
                    *fptr++ = coords[coord_index];
                    *fptr++ = coords[coord_index+1];
                    *fptr++ = coords[coord_index+2];

                    *fptr++ = normals[normal_index];
                    *fptr++ = normals[normal_index+1];
                    *fptr++ = normals[normal_index+2];

                    q_indices.push_back( q_indices.size() );
                }
            }
        }
    }
    A3DTess3DGet( nullptr, &t3dd );
    A3DTessBaseGet( nullptr, &tbd );

    auto buf = new Qt3DCore::QBuffer();
    buf->setData( bufferBytes );

    auto geometry = new QGeometry;
    auto position_attribute = new QAttribute( buf, QAttribute::defaultPositionAttributeName(), QAttribute::Float, 3, q_indices.size(), 0, stride );
    geometry->addAttribute( position_attribute );
        
    auto normal_attribute = new QAttribute( buf, QAttribute::defaultNormalAttributeName(), QAttribute::Float, 3, q_indices.size(), sizeof(float) * 3, stride );
    geometry->addAttribute( normal_attribute );

    
    QByteArray indexBytes;
    QAttribute::VertexBaseType ty;
    if (q_indices.size() < 65536) {
        // we can use USHORT
        ty = QAttribute::UnsignedShort;
        indexBytes.resize(q_indices.size() * sizeof(quint16));
        quint16 *usptr = reinterpret_cast<quint16*>(indexBytes.data());
        for (int i = 0; i < int(q_indices.size()); ++i)
            *usptr++ = static_cast<quint16>(q_indices.at(i));
    } else {
        // use UINT - no conversion needed, but let's ensure int is 32-bit!
        ty = QAttribute::UnsignedInt;
        Q_ASSERT(sizeof(int) == sizeof(quint32));
        indexBytes.resize(q_indices.size() * sizeof(quint32));
        memcpy(indexBytes.data(), reinterpret_cast<const char*>(q_indices.data()), indexBytes.size());
    }

    auto *indexBuffer = new Qt3DCore::QBuffer();
    indexBuffer->setData( indexBytes );
    QAttribute *indexAttribute = new QAttribute(indexBuffer, ty, 1, q_indices.size());
    indexAttribute->setAttributeType(QAttribute::IndexAttribute);
    geometry->addAttribute(indexAttribute);

    auto renderer = new Qt3DRender::QGeometryRenderer();
    renderer->setGeometry( geometry );

    return renderer;
}
