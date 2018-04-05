#include <vtkSlicerImageFusionLogic.h>

#include <vtkMRMLThreeDViewDisplayableManagerFactory.h>

#include "qSlicerImageFusionModule.h"
#include "qSlicerImageFusionModuleWidget.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkSlicerImageFusionModuleMRMLDisplayableManager)

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QtPlugin>
Q_EXPORT_PLUGIN2(qSlicerImageFusionModule, qSlicerImageFusionModule);
#endif

class qSlicerImageFusionModulePrivate {
public:
    qSlicerImageFusionModulePrivate();
};

qSlicerImageFusionModulePrivate::qSlicerImageFusionModulePrivate() {}

qSlicerImageFusionModule::qSlicerImageFusionModule(QObject *_parent)
    : Superclass(_parent), d_ptr(new qSlicerImageFusionModulePrivate) {}

qSlicerImageFusionModule::~qSlicerImageFusionModule() {}

QString
qSlicerImageFusionModule::helpText() const {
    return "This is a loadable module that can be bundled in an extension";
}

QString
qSlicerImageFusionModule::acknowledgementText() const {
    return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

QStringList
qSlicerImageFusionModule::contributors() const {
    QStringList moduleContributors;
    moduleContributors << QString("John Doe (AnyWare Corp.)");
    return moduleContributors;
}

QIcon
qSlicerImageFusionModule::icon() const {
    return QIcon(":/Icons/ImageFusion.png");
}

QStringList
qSlicerImageFusionModule::categories() const {
    return QStringList() << "Examples";
}

QStringList
qSlicerImageFusionModule::dependencies() const {
    return QStringList();
}

void
qSlicerImageFusionModule::setup() {
    this->Superclass::setup();
    vtkMRMLThreeDViewDisplayableManagerFactory::GetInstance()->RegisterDisplayableManager(
        "vtkMRMLImageFusionDisplayableManager");
}

qSlicerAbstractModuleRepresentation *
qSlicerImageFusionModule ::createWidgetRepresentation() {
    return new qSlicerImageFusionModuleWidget;
}

vtkMRMLAbstractLogic *
qSlicerImageFusionModule::createLogic() {
    return vtkSlicerImageFusionLogic::New();
}
