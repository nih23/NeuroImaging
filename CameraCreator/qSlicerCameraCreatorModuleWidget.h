#pragma once

#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerCameraCreatorModuleExport.h"

#include <vtkMRMLNode.h>

class qSlicerCameraCreatorModuleWidgetPrivate;

/**
 * @brief UI representation of the Camera Creator module.
 *
 * Hands over all inputs to the logic of the module.
 */
class Q_SLICER_QTMODULES_CAMERACREATOR_EXPORT qSlicerCameraCreatorModuleWidget
    : public qSlicerAbstractModuleWidget {
    Q_OBJECT
    QVTK_OBJECT

public:
    typedef qSlicerAbstractModuleWidget Superclass;

    /**
     * @brief Creates a new qSlicerCameraCreatorModuleWidget.
     * @param parent Parent that owns the new object
     */
    qSlicerCameraCreatorModuleWidget(QWidget *parent = 0);

    virtual ~qSlicerCameraCreatorModuleWidget();

protected slots:
    /**
     * @brief Handles a selection change of the target camera node
     * @param node New camera node
     */
    void
    onCameraNodeChanged(vtkMRMLNode *node);

    /**
     * @brief Handles a selection change of the linear transform node that holds the transform from the
     * reference adapter to the anatomy
     * @param node New linear transform node
     */
    void
    onReferenceToRASTransformNodeChanged(vtkMRMLNode *node);

    /**
     * @brief Handles a change in the linear transform node that holds the transform from the
     * camera to the reference adapter.
     * @param node New linear transform node
     */
    void
    onCameraToReferenceTransformNodeChanged(vtkMRMLNode *node);

    /**
     * @brief Handles a parameter change of the view up vector
     */
    void
    onViewUpChanged();

    /**
     * @brief Handles a parameter change of the view direction vector
     */
    void
    onViewDirectionChanged();

    /**
     * @brief Handles a parameter change of the vertical view angle
     */
    void
    onViewAngleChanged(double value);

    /**
     * @brief Handles an 'Apply' button press
     */
    void
    onApplied();

protected:
    /**
     * @brief Private implementation that holds all data
     */
    QScopedPointer<qSlicerCameraCreatorModuleWidgetPrivate> d_ptr;

    /**
     * @brief Sets up the UI representation. This is called when the module is opened for the first
     * time.
     */
    virtual void
    setup();

private:
    Q_DECLARE_PRIVATE(qSlicerCameraCreatorModuleWidget);
    Q_DISABLE_COPY(qSlicerCameraCreatorModuleWidget);
};
