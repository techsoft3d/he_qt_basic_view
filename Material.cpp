#include <Qt3DExtras>
#include "Material.h"


namespace {
    QColor getColor( A3DUns32 const &color_idx, int const &a ) {
        if( A3D_DEFAULT_COLOR_INDEX == color_idx ) {
            return QColor( 255, 0, 0 );
        }

        A3DGraphRgbColorData rgb_color_data;
        A3D_INITIALIZE_DATA( A3DGraphRgbColorData, rgb_color_data );
        A3DGlobalGetGraphRgbColorData( color_idx, &rgb_color_data );
        auto const &r = rgb_color_data.m_dRed;
        auto const &g = rgb_color_data.m_dGreen;
        auto const &b = rgb_color_data.m_dBlue;
        return QColor( static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255), a);
    }
}

Qt3DCore::QComponent *createMaterial( A3DGraphStyleData const &style_data ) {
    auto material = new Qt3DExtras::QDiffuseSpecularMaterial();
    material->setDiffuse(QColor("red"));
    if( style_data.m_bMaterial ) {
        A3DBool is_texuture = false;
        A3DGlobalIsMaterialTexture( style_data.m_uiRgbColorIndex, &is_texuture );
        if( ! is_texuture ) {
            A3DGraphMaterialData material_data;
            A3D_INITIALIZE_DATA( A3DGraphMaterialData, material_data );
            A3DGlobalGetGraphMaterialData( style_data.m_uiRgbColorIndex, &material_data );
            auto const ambient_color = getColor( material_data.m_uiAmbient, static_cast<int>(255 * material_data.m_dAmbientAlpha) );
            auto const diffuse_color = getColor( material_data.m_uiDiffuse, static_cast<int>(255 * material_data.m_dDiffuseAlpha) );
            if( ambient_color.alpha() == 255 && diffuse_color.alpha() == 0 ) {
                material->setDiffuse( ambient_color );
            } else if( ambient_color.alpha() == 0 && diffuse_color.alpha() == 255 ) {
                material->setDiffuse( diffuse_color );
            }
            material->setSpecular(getColor( material_data.m_uiSpecular, material_data.m_dSpecularAlpha ));
        }
    } else {
        auto const a = style_data.m_bIsTransparencyDefined ? style_data.m_ucTransparency : 255u;
        material->setDiffuse(getColor( style_data.m_uiRgbColorIndex, a ));
    }
    return material;
}