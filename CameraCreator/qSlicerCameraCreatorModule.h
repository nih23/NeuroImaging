#pragma once

#include "qSlicerLoadableModule.h"

#include "qSlicerCameraCreatorModuleExport.h"

class qSlicerCameraCreatorModulePrivate;

/**
 * @brief Camera Creator module
 *
 * Instantiates the widget and the logic of the module
 */
class Q_SLICER_QTMODULES_CAMERACREATOR_EXPORT qSlicerCameraCreatorModule
    : public qSlicerLoadableModule {
    Q_OBJECT
#ifdef Slicer_HAVE_QT5
    Q_PLUGIN_METADATA(IID "org.slicer.modules.loadable.qSlicerLoadableModule/1.0");
#endif
    Q_INTERFACES(qSlicerLoadableModule);

public:
    typedef qSlicerLoadableModule Superclass;

    explicit qSlicerCameraCreatorModule(QObject *parent = 0);

    virtual ~qSlicerCameraCreatorModule();

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
    QScopedPointer<qSlicerCameraCreatorModulePrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(qSlicerCameraCreatorModule);
    Q_DISABLE_COPY(qSlicerCameraCreatorModule);
};
