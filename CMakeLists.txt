cmake_minimum_required(VERSION 3.5)

find_package(Slicer COMPONENTS ConfigurePrerequisites REQUIRED)

project(NeuroImaging)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "http://slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/NeuroImaging")
set(EXTENSION_CATEGORY "Visualisation")
set(EXTENSION_CONTRIBUTORS "Tobias Hänel (TU Dresden)")
set(EXTENSION_DESCRIPTION "Creates a multimodal visualisation of MRI and thermal images.

Thermal images are streamed using OpenIGTLink.
Position and orientation of a reference adapter that is mounted to the thermographic camera is streamed using OpenIGTLink.

A 3D visualisation of the thermal images is created by reprojecting the thermal image pixels onto a surface that is extracted out of the MRI images.

The reprojected thermal visualisation is combined with an arbitrary MRI visualisation using methods of image fusion.")
set(EXTENSION_ICONURL "http://www.example.com/Slicer/Extensions/NeuroImaging.png")
set(EXTENSION_SCREENSHOTURLS "http://www.example.com/Slicer/Extensions/NeuroImaging/Screenshots/1.png")
set(EXTENSION_DEPENDS "") # Specified as a space separated string, a list or 'NA' if any

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

find_package(OpenCV REQUIRED)

#set(SlicerOpenIGTLink_DIR "/home/tobias/Programme/SlicerOpenIGTLink-SuperBuild-Release/inner-build")
#find_package(SlicerOpenIGTLink REQUIRED)
#include(${OpenIGTLink_USE_FILE})

#-----------------------------------------------------------------------------
# Extension modules
#add_subdirectory(ImageWithFocusOpenIGTLink)
add_subdirectory(ProjectiveTextureMapping)
add_subdirectory(ImageFusion)
add_subdirectory(CameraCreator)
## NEXT_MODULE

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
