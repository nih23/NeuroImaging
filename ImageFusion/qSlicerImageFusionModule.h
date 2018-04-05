#pragma once

#include "qSlicerLoadableModule.h"

#include "qSlicerImageFusionModuleExport.h"

class qSlicerImageFusionModulePrivate;

/**
 * @brief Image fusion module
 *
 * Instantiates the widget and the logic of the module and registers the
 * vtkMRMLImageFusionDisplayableManager class.
 */
class Q_SLICER_QTMODULES_IMAGEFUSION_EXPORT qSlicerImageFusionModule
    : public qSlicerLoadableModule {
    Q_OBJECT
#ifdef Slicer_HAVE_QT5
    Q_PLUGIN_METADATA(IID "org.slicer.modules.loadable.qSlicerLoadableModule/1.0");
#endif
    Q_INTERFACES(qSlicerLoadableModule);

public:
    typedef qSlicerLoadableModule Superclass;

    explicit qSlicerImageFusionModule(QObject *parent = 0);

    virtual ~qSlicerImageFusionModule();

    qSlicerGetTitleMacro(QTMODULE_TITLE);

    virtual QString
    helpText() const;

    virtual QString
    acknowledgementText() const;

    virtual QStringList
    contributors() const;

    virtual QIcon
    icon() const;

    virtual QStringList
    categories() const;

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
    QScopedPointer<qSlicerImageFusionModulePrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(qSlicerImageFusionModule);
    Q_DISABLE_COPY(qSlicerImageFusionModule);
};
