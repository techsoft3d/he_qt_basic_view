#define INITIALIZE_A3D_API
#include <A3DSDKIncludes.h>
#include <QtGui>
#include <Qt3DCore>
#include <Qt3DExtras>
#include <QtWidgets>

#include "Scene.h"
#include "Transform.h"

#define xstr(s) __str(s)
#define __str(s) #s

static QString const exchange_bin_dir = xstr(EXCHANGE_BIN_DIR);
static QString const exchange_data_dir = xstr(EXCHANGE_DATA_DIR);

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Load and license HOOPS Exchange
#ifdef _MSC_VER
    auto loader = std::make_unique<A3DSDKHOOPSExchangeLoader>( qUtf16Printable( exchange_bin_dir ) );
#else
    auto loader = std::make_unique<A3DSDKHOOPSExchangeLoader>( qUtf8Printable( exchange_bin_dir ) );
#endif

    if(!loader->m_bSDKLoaded) {
        QMessageBox::warning( nullptr, "Exchange Error", QString("Unable to initialize HOOPS Exchange.\nTried: %1").arg(exchange_bin_dir) );
        return -1;
    }

    // Declare and initialize the file loading options
    A3DRWParamsLoadData load_params;
    A3D_INITIALIZE_DATA(A3DRWParamsLoadData, load_params);
    load_params.m_sGeneral.m_bReadSolids = true;
    load_params.m_sGeneral.m_eReadGeomTessMode = kA3DReadGeomOnly;

    // Allow the user to choose which file to load
    A3DAsmModelFile *model_file = nullptr;
    do {
        // Pick a file using the standard file open dialog
        auto const input_file = QFileDialog::getOpenFileName( nullptr, "Open CAD File", exchange_data_dir );
        if(input_file.isEmpty()) {
            // If the user clicked cancel, exit.
            return 0;
        }

        // Attempt to load the file
        auto const load_status = A3DAsmModelFileLoadFromFile( qUtf8Printable( input_file ), &load_params, &model_file );
        if( A3D_SUCCESS != load_status ) {
            // If unsuccessful, show a warning message
            QMessageBox::warning( nullptr, "Exchange Error",
            QString( "Unable to load the specified file.\nStatus = %1").arg(A3DMiscGetErrorMsg(load_status)));
        }
    } while(nullptr == model_file );

    // Create the main window
    Qt3DExtras::Qt3DWindow view;

    // Create the Qt3D scene from the loaded model
    Qt3DCore::QEntity *scene = createScene(model_file);
    view.setRootEntity(scene);

    // Determine the loaded model's bounding box
    A3DBoundingBoxData bbd;
    A3D_INITIALIZE_DATA(A3DBoundingBoxData, bbd);
    auto const invalid_bb = bbd;
    if( A3D_SUCCESS != A3DMiscGetBoundingBox( model_file, &bbd ) || 0 == memcmp( &invalid_bb, &bbd, sizeof( A3DBoundingBoxData) ) ) {
        A3DMiscComputeBoundingBox( model_file, nullptr, &bbd );
    }

    // Compute some values based on the bounding box
    auto const mn = toQVector3D( bbd.m_sMin );
    auto const mx = toQVector3D(bbd.m_sMax);
    auto const diagonal = mx - mn;
    auto const l = diagonal.length();
    auto const mid_pt = mn + diagonal * 0.5;

    // Set up the camera to properly contain the model bounds
    Qt3DRender::QCamera *camera = view.camera();
    auto onResize = [&]() {
        auto const sz = view.size();
        auto const aspect = static_cast<float>(sz.width()) / static_cast<float>(sz.height());
        camera->lens()->setOrthographicProjection( -l * aspect, l * aspect, -l, l, -2*l, 2*l );
        camera->setPosition(mid_pt + QVector3D( 0.0, 0.0, l ) );
        camera->setViewCenter(mid_pt);
    };

    // Initialize the camera, and ensure it is updated when the view is resized
    onResize();
    QObject::connect( &view, &QWindow::widthChanged, onResize );
    QObject::connect( &view, &QWindow::heightChanged, onResize );
 
    // Add a light to the scene
    auto light_node = new Qt3DCore::QEntity( scene );
    light_node->addComponent( new Qt3DRender::QPointLight() );
    auto light_xform = new Qt3DCore::QTransform();
    light_node->addComponent(light_xform);
    light_xform->setTranslation( camera->position() );
    
    // Update the light's position whenever the camera changes
    QObject::connect( camera, &Qt3DRender::QCamera::positionChanged, [&](QVector3D const &position ) {
        light_xform->setTranslation( position );
    });
    
    // Add camera controls
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(scene);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera);

    view.show();

    return app.exec();
}
