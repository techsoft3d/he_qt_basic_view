#pragma once

#include <QtCore>
#include <A3DSDKIncludes.h>

using EntityArray = QVector<A3DEntity*>;

struct CascadedAttributes {
    CascadedAttributes( EntityArray const &path ) {
        // Create a vector to hold the cascaded attributes handles
        QVector<A3DMiscCascadedAttributes*> cascaded_attribs;

        // Create the "root" cascaded attribute handle
        cascaded_attribs.push_back( nullptr );
        A3DMiscCascadedAttributesCreate( &cascaded_attribs.back() );

        // for each entity in the path,
        for( auto ntt : path ) {
            if( A3DEntityIsBaseWithGraphicsType( ntt ) ) {
                // get the handle to the previous cascaded attributes
                auto father = cascaded_attribs.back();

                // create a new cascaded attributes handle for this entity
                cascaded_attribs.push_back( nullptr );
                A3DMiscCascadedAttributesCreate( &cascaded_attribs.back() );

                // push this handle onto the stack
                A3DMiscCascadedAttributesPush( cascaded_attribs.back(), ntt, father );
            }
        }

        // Compute the cascaded attributes data
        A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, d);
        A3DMiscCascadedAttributesGet( cascaded_attribs.back(), &d );

        for( auto attrib : cascaded_attribs ) {
            A3DMiscCascadedAttributesDelete( attrib );
        }
    }

    ~CascadedAttributes( void ) {
        A3DMiscCascadedAttributesGet( nullptr, &d );
    }

    A3DMiscCascadedAttributesData const *operator->( void ) {
        return &d;
    }

    A3DMiscCascadedAttributesData d;
};