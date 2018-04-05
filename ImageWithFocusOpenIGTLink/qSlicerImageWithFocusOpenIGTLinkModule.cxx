#include "vtkSlicerImageWithFocusOpenIGTLinkLogic.h"

#include <QDebug>

#include <qSlicerCoreApplication.h>
#include <qSlicerModuleManager.h>

#include "qSlicerImageWithFocusOpenIGTLinkModule.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QtPlugin>
Q_EXPORT_PLUGIN2(qSlicerImageWithFocusOpenIGTLinkModule, qSlicerImageWithFocusOpenIGTLinkModule);
#endif

class qSlicerImageWithFocusOpenIGTLinkModulePrivate {
public:
    qSlicerImageWithFocusOpenIGTLinkModulePrivate();
};

qSlicerImageWithFocusOpenIGTLinkModulePrivate::qSlicerImageWithFocusOpenIGTLinkModulePrivate() {}

qSlicerImageWithFocusOpenIGTLinkModule::qSlicerImageWithFocusOpenIGTLinkModule(QObject *_parent)
    : Superclass(_parent), d_ptr(new qSlicerImageWithFocusOpenIGTLinkModulePrivate) {}

qSlicerImageWithFocusOpenIGTLinkModule::~qSlicerImageWithFocusOpenIGTLinkModule() {}

QString
qSlicerImageWithFocusOpenIGTLinkModule::acknowledgementText() const {
    return "";
}

QStringList
qSlicerImageWithFocusOpenIGTLinkModule::contributors() const {
    QStringList moduleContributors;
    moduleContributors << QString("Tobias Haenel (TU Dresden)");
    return moduleContributors;
}

QStringList
qSlicerImageWithFocusOpenIGTLinkModule::dependencies() const {
    return QStringList() << "OpenIGTLinkIF";
}

void
qSlicerImageWithFocusOpenIGTLinkModule::setup() {
    this->Superclass::setup();

    this->setWidgetRepresentationCreationEnabled(false);

    auto *moduleLogic = vtkSlicerImageWithFocusOpenIGTLinkLogic::SafeDownCast(this->logic());
    qSlicerAbstractCoreModule *openIGTLinkIFModule =
        qSlicerCoreApplication::application()->moduleManager()->module("OpenIGTLinkIF");
    if (openIGTLinkIFModule) {
        auto *openIGTLinkIFLogic =
            vtkSlicerOpenIGTLinkIFLogic::SafeDownCast(openIGTLinkIFModule->logic());
        moduleLogic->SetOpenIGTLinkIFLogic(openIGTLinkIFLogic);
    } else {
        qWarning() << "No OpenIGTLinkIF module found" << endl;
    }
}

qSlicerAbstractModuleRepresentation *
qSlicerImageWithFocusOpenIGTLinkModule::createWidgetRepresentation() {
    return nullptr;
}

vtkMRMLAbstractLogic *
qSlicerImageWithFocusOpenIGTLinkModule::createLogic() {
    return vtkSlicerImageWithFocusOpenIGTLinkLogic::New();
}
