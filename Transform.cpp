#include "Transform.h"

namespace {    
    QMatrix4x4 getMatrixFromCartesian( A3DMiscCartesianTransformation *xform ) {
		A3DMiscCartesianTransformationData d;
        A3D_INITIALIZE_DATA(A3DMiscCartesianTransformationData, d);
        A3DMiscCartesianTransformationGet( xform, &d );
		auto const mirror = (d.m_ucBehaviour & kA3DTransformationMirror) ? -1. : 1.;
		auto const s = toQVector3D( d.m_sScale );
		auto const o = toQVector3D( d.m_sOrigin );
		auto const x = toQVector3D( d.m_sXVector );
		auto const y = toQVector3D( d.m_sYVector );
		auto const z = QVector3D::crossProduct( x, y ) * mirror;
        A3DMiscCartesianTransformationGet( nullptr, &d );
		return QMatrix4x4(
            x.x() * s.x(), y.x() * s.y(), z.x() * s.z(), o.x(),
            x.y() * x.x(), y.y() * s.y(), z.y() * s.z(), o.y(),
            x.z() * s.x(), y.z() * s.y(), z.z() * s.z(), o.z(),
            0.f,           0.f,           0.f,           1.f 
        );
	}

	QMatrix4x4 getMatrixFromGeneralTransformation( A3DMiscGeneralTransformation *xform ) {
        A3DMiscGeneralTransformationData d;
        A3D_INITIALIZE_DATA(A3DMiscGeneralTransformationData, d);
        A3DMiscGeneralTransformationGet( xform, &d );

		auto const coeff = d.m_adCoeff;
		QMatrix4x4 result;
        for( auto row = 0u; row < 4u; ++row ) {
            for( auto col = 0u; col < 4u; ++col ) {
                result(row,col) = static_cast<float>(coeff[row + col * 4]);
            }
        }
		return result;
	}

    QMatrix4x4 toMatrix( A3DMiscTransformation *xfrm ) {
        if( xfrm ) {
            A3DEEntityType xfrm_type = kA3DTypeUnknown;
            A3DEntityGetType( xfrm, &xfrm_type );
            switch( xfrm_type ) {
                case kA3DTypeMiscCartesianTransformation:
                    return getMatrixFromCartesian( xfrm );
                    break;
                case kA3DTypeMiscGeneralTransformation:
                    return getMatrixFromGeneralTransformation( xfrm );
                    break;
                default:
                    throw std::invalid_argument( "Unexpected type." );
                    break;
            }
        }
        return QMatrix4x4();
    }

    A3DMiscTransformation *getTransform( A3DEntity *ntt ) {
        A3DMiscTransformation *result = nullptr;
        A3DEEntityType ntt_type = kA3DTypeUnknown;
        A3DEntityGetType(ntt, &ntt_type );
        if( kA3DTypeAsmProductOccurrence == ntt_type ) {
            A3DAsmProductOccurrenceData d;
            A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, d);
            A3DAsmProductOccurrenceGet( ntt, &d );
            result = d.m_pLocation ? d.m_pLocation : getTransform( d.m_pPrototype );
            A3DAsmProductOccurrenceGet( nullptr, &d );
        } else if( ntt_type > kA3DTypeRi && ntt_type <= kA3DTypeRiCoordinateSystemItem) {
            A3DRiRepresentationItemData d;
            A3D_INITIALIZE_DATA(A3DRiRepresentationItemData, d);
            A3DRiRepresentationItemGet( ntt, &d );
            if( auto ti_cs = d.m_pCoordinateSystem ) {
                A3DRiCoordinateSystemData cs_d;
                A3D_INITIALIZE_DATA(A3DRiCoordinateSystemData, cs_d);
                A3DRiCoordinateSystemGet( d.m_pCoordinateSystem, &cs_d );
                result = cs_d.m_pTransformation;
                A3DRiCoordinateSystemGet( nullptr, &cs_d );
            }
            A3DRiRepresentationItemGet( nullptr, &d );
        }
        return result;
    }
}

Qt3DCore::QTransform *createTransform( EntityArray const &path ) {
    QMatrix4x4 net_matrix;
    for( auto const ntt : path ) {
        A3DMiscTransformation *xform = getTransform( ntt );
        net_matrix *= toMatrix( xform );
    }
    auto xform = new Qt3DCore::QTransform();
    xform->setMatrix( net_matrix );
    return xform;
}