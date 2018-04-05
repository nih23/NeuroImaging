/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// ProjectiveTextureMapping Logic includes
#include <vtkSlicerProjectiveTextureMappingLogic.h>

#include <vtkMRMLThreeDViewDisplayableManagerFactory.h>

// ProjectiveTextureMapping includes
#include "qSlicerProjectiveTextureMappingModule.h"
#include "qSlicerProjectiveTextureMappingModuleWidget.h"

// DisplayableManager initialization
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkSlicerProjectiveTextureMappingModuleMRMLDisplayableManager)

//-----------------------------------------------------------------------------
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QtPlugin>
Q_EXPORT_PLUGIN2(qSlicerProjectiveTextureMappingModule, qSlicerProjectiveTextureMappingModule);
#endif

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerProjectiveTextureMappingModulePrivate {
public:
    qSlicerProjectiveTextureMappingModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerProjectiveTextureMappingModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerProjectiveTextureMappingModulePrivate::qSlicerProjectiveTextureMappingModulePrivate() {}

//-----------------------------------------------------------------------------
// qSlicerProjectiveTextureMappingModule methods

//-----------------------------------------------------------------------------
qSlicerProjectiveTextureMappingModule::qSlicerProjectiveTextureMappingModule(QObject *_parent)
    : Superclass(_parent), d_ptr(new qSlicerProjectiveTextureMappingModulePrivate) {}

//-----------------------------------------------------------------------------
qSlicerProjectiveTextureMappingModule::~qSlicerProjectiveTextureMappingModule() {}

//-----------------------------------------------------------------------------
QString
qSlicerProjectiveTextureMappingModule::helpText() const {
    return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString
qSlicerProjectiveTextureMappingModule::acknowledgementText() const {
    return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList
qSlicerProjectiveTextureMappingModule::contributors() const {
    QStringList moduleContributors;
    moduleContributors << QString("John Doe (AnyWare Corp.)");
    return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon
qSlicerProjectiveTextureMappingModule::icon() const {
    return QIcon(":/Icons/ProjectiveTextureMapping.png");
}

//-----------------------------------------------------------------------------
QStringList
qSlicerProjectiveTextureMappingModule::categories() const {
    return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList
qSlicerProjectiveTextureMappingModule::dependencies() const {
    return QStringList();
}

QStringList
qSlicerProjectiveTextureMappingModule::associatedNodeTypes() const {
    return QStringList() << "vtkMRMLProjectiveTextureMappingDisplayNode";
}

//-----------------------------------------------------------------------------
void
qSlicerProjectiveTextureMappingModule::setup() {
    this->Superclass::setup();
    vtkMRMLThreeDViewDisplayableManagerFactory::GetInstance()->RegisterDisplayableManager(
        "vtkMRMLProjectiveTextureMappingDisplayableManager");
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation *
qSlicerProjectiveTextureMappingModule ::createWidgetRepresentation() {
    return new qSlicerProjectiveTextureMappingModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic *
qSlicerProjectiveTextureMappingModule::createLogic() {
    return vtkSlicerProjectiveTextureMappingLogic::New();
}
