#pragma once

#include "qSlicerLoadableModule.h"

#include "qSlicerProjectiveTextureMappingModuleExport.h"

class qSlicerProjectiveTextureMappingModulePrivate;


/**
 * @brief Projective texture mapping module
 *
 * Instantiates the widget and the logic of the module and registers the
 * vtkMRMLImageFusionDisplayableManager class.
*/
class Q_SLICER_QTMODULES_PROJECTIVETEXTUREMAPPING_EXPORT qSlicerProjectiveTextureMappingModule
    : public qSlicerLoadableModule {
    Q_OBJECT
#ifdef Slicer_HAVE_QT5
    Q_PLUGIN_METADATA(IID "org.slicer.modules.loadable.qSlicerLoadableModule/1.0");
#endif
    Q_INTERFACES(qSlicerLoadableModule);

public:
    typedef qSlicerLoadableModule Superclass;

    explicit qSlicerProjectiveTextureMappingModule(QObject *parent = 0);

    virtual ~qSlicerProjectiveTextureMappingModule();

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

    virtual QStringList
    associatedNodeTypes() const;

protected:
    virtual void
    setup();

    virtual qSlicerAbstractModuleRepresentation *
    createWidgetRepresentation();

    virtual vtkMRMLAbstractLogic *
    createLogic();

protected:
    QScopedPointer<qSlicerProjectiveTextureMappingModulePrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(qSlicerProjectiveTextureMappingModule);
    Q_DISABLE_COPY(qSlicerProjectiveTextureMappingModule);
};
