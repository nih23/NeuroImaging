#pragma once

#include "qSlicerLoadableModule.h"

#include "qSlicerImageWithFocusOpenIGTLinkModuleExport.h"

class qSlicerImageWithFocusOpenIGTLinkModulePrivate;

/**
 * @brief ImageWithFocusOpenIGTLink module
 *
 * This module adds support for the custom image message of Florian Weidners OpenIGTLink
 * imaging server. This class makes sure that the logic gets a reference to the OpenIGTLinkIF
 * module, to register its own message converter.
 */
class Q_SLICER_QTMODULES_IMAGEWITHFOCUSOPENIGTLINK_EXPORT qSlicerImageWithFocusOpenIGTLinkModule
    : public qSlicerLoadableModule {
    Q_OBJECT
#ifdef Slicer_HAVE_QT5
    Q_PLUGIN_METADATA(IID "org.slicer.modules.loadable.qSlicerLoadableModule/1.0");
#endif
    Q_INTERFACES(qSlicerLoadableModule);

public:
    typedef qSlicerLoadableModule Superclass;
    explicit qSlicerImageWithFocusOpenIGTLinkModule(QObject *parent = 0);
    virtual ~qSlicerImageWithFocusOpenIGTLinkModule();

    qSlicerGetTitleMacro(QTMODULE_TITLE);

    virtual QString
    acknowledgementText() const;

    virtual QStringList
    contributors() const;

    virtual QStringList
    dependencies() const;

protected:
    virtual void
    setup();

    virtual qSlicerAbstractModuleRepresentation *
    createWidgetRepresentation();

    virtual vtkMRMLAbstractLogic *
    createLogic();

protected:
    QScopedPointer<qSlicerImageWithFocusOpenIGTLinkModulePrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(qSlicerImageWithFocusOpenIGTLinkModule);
    Q_DISABLE_COPY(qSlicerImageWithFocusOpenIGTLinkModule);
};
