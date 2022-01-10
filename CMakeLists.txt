cmake_minimum_required(VERSION 3.13.4)
project(qt_he_viewer VERSION 1.0.0 LANGUAGES CXX)



#######################################################
###  THIS IS THE ONLY SECTION THAT MUST BE MODIFIED ###
#######################################################
if(MSVC)
  # This is the location of your Windows HOOPS Exchange installation
  set(HOOPS_EXCHANGE_DIR "X:/HOOPS_Exchange_Publish_2021_SP2_U2")
else(MSVC)
  if(APPLE)
    # This is the location of your macOS HOOPS Exchange installation
    set(HOOPS_EXCHANGE_DIR "/opt/local/ts3d/HOOPS_Exchange_2021_SP2_U2")
  else(APPLE)
    # This is the location of your Linux HOOPS Exchange installation
    set(HOOPS_EXCHANGE_DIR "/opt/local/ts3d/HOOPS_Exchange_Publish_2021_SP2_U2")
  endif(APPLE)
endif(MSVC)
#######################################################


if(MSVC)
  set(exchange_arch "win64_v140")
else(MSVC)
  if(APPLE)
    set(exchange_arch "macos")
  else(APPLE)
      set(exchange_arch "linux64")
  endif(APPLE)
endif(MSVC)


string(REPLACE "\\" "/" exchange_dir_def ${HOOPS_EXCHANGE_DIR})
add_definitions(-DEXCHANGE_BIN_DIR=${exchange_dir_def}/bin/${exchange_arch})
add_definitions(-DEXCHANGE_DATA_DIR=${exchange_dir_def}/samples/data)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Widgets)
find_package(Qt6 COMPONENTS 3DCore)
find_package(Qt6 COMPONENTS 3DRender)
find_package(Qt6 COMPONENTS 3DInput)
find_package(Qt6 COMPONENTS 3DExtras)
find_package(Qt6 COMPONENTS Concurrent)

add_executable(qt_he_viewer main.cpp ForEachRepresentationItem.cpp Mesh.cpp Scene.cpp Material.cpp Transform.cpp)


target_link_libraries(qt_he_viewer PUBLIC
    Qt::3DCore
    Qt::3DExtras
    Qt::3DInput
    Qt::3DRender
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

include_directories("${HOOPS_EXCHANGE_DIR}/include")

if(!MSVC)
  target_link_libraries(qt_viewer PRIVATE dl)
endif(!MSVC)