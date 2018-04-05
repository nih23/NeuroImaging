#include "qSlicerCameraCreatorModule.h"

#include <vtkSlicerCameraCreatorLogic.h>

#include "qSlicerCameraCreatorModuleWidget.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QtPlugin>
Q_EXPORT_PLUGIN2(qSlicerCameraCreatorModule, qSlicerCameraCreatorModule);
#endif

class qSlicerCameraCreatorModulePrivate {
public:
    qSlicerCameraCreatorModulePrivate();
};

qSlicerCameraCreatorModulePrivate::qSlicerCameraCreatorModulePrivate() {}

qSlicerCameraCreatorModule::qSlicerCameraCreatorModule(QObject *_parent)
    : Superclass(_parent), d_ptr(new qSlicerCameraCreatorModulePrivate) {}

qSlicerCameraCreatorModule::~qSlicerCameraCreatorModule() {}

QString
qSlicerCameraCreatorModule::helpText() const {
    return "This is a loadable module that can be bundled in an extension";
}

QString
qSlicerCameraCreatorModule::acknowledgementText() const {
    return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

QStringList
qSlicerCameraCreatorModule::contributors() const {
    QStringList moduleContributors;
    moduleContributors << QString("John Doe (AnyWare Corp.)");
    return moduleContributors;
}

QIcon
qSlicerCameraCreatorModule::icon() const {
    return QIcon(":/Icons/CameraCreator.png");
}

QStringList
qSlicerCameraCreatorModule::categories() const {
    return QStringList() << "Examples";
}

QStringList
qSlicerCameraCreatorModule::dependencies() const {
    return QStringList();
}

void
qSlicerCameraCreatorModule::setup() {
    this->Superclass::setup();
}

qSlicerAbstractModuleRepresentation *
qSlicerCameraCreatorModule ::createWidgetRepresentation() {
    return new qSlicerCameraCreatorModuleWidget;
}

vtkMRMLAbstractLogic *
qSlicerCameraCreatorModule::createLogic() {
    return vtkSlicerCameraCreatorLogic::New();
}
