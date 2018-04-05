#pragma once

#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerImageFusionModuleExport.h"

class qSlicerImageFusionModuleWidgetPrivate;
class vtkMRMLNode;
class vtkMRMLImageFusionDisplayNode;
class vtkMRMLVolumeRenderingDisplayNode;
class vtkMRMLProjectiveTextureMappingDisplayNode;

/**
 * @brief UI representation of the Image Fusion module
 *
 * Hands over all inputs to the selected image fusion display node.
 */
class Q_SLICER_QTMODULES_IMAGEFUSION_EXPORT qSlicerImageFusionModuleWidget
    : public qSlicerAbstractModuleWidget {
    Q_OBJECT
    QVTK_OBJECT
public:

    typedef qSlicerAbstractModuleWidget Superclass;

    /**
     * @brief Creates a new qSlicerImageFusionModuleWidget.
     * @param parent Parent that owns the new object
     */
    qSlicerImageFusionModuleWidget(QWidget *parent = 0);

    /**
     * @brief Destroys the qSlicerImageFusionModuleWidget.
     */
    virtual ~qSlicerImageFusionModuleWidget();

    /**
     * @brief Set an edited node for the module. This can be used by other modules.
     * @param node Image fusion display node
     * @param role Role of the node
     * @param context Context of the node
     * @return Status
     */
    virtual bool
    setEditedNode(vtkMRMLNode *node, QString role = QString(), QString context = QString());

    /**
     * @brief Gets a certainty that this module can edit a given node.
     * @note This module can edit image fusion display nodes
     * @param node Checked node
     * @return Certainty value between 0 and 1
     */
    virtual double
    nodeEditable(vtkMRMLNode *node);

protected slots:
    /**
     * @brief Handles a selection change of the target image fusion display node.
     * @param node New image fusion display node
     */
    void
    onCurrentMRMLImageFusionDisplayNodeChanged(vtkMRMLNode *node);

    /**
     * @brief Handles a selection change of the fused volume rendering display node.
     * @param node New volume rendering display node
     */
    void
    onCurrentMRMLVolumeRenderingDisplayNodeChanged(vtkMRMLNode *node);

    /**
     * @brief Handles a selection change of the fused projective texture mapping display node.
     * @param node New projective texture mapping display node
     */
    void
    onCurrentMRMLProjectiveTextureMappingDisplayNodeChanged(vtkMRMLNode *node);

    /**
     * @brief Handles a toggle of the weighted averaging radio button.
     * @param status New selection status of the radio button
     */
    void
    onWeightedAveragingToggled(bool status);

    /**
     * @brief Handles a toggle of the guided filtering radio button.
     * @param status New selection status of the radio button
     */
    void
    onGuidedFilteringToggled(bool status);

    /**
     * @brief Handles a change in the weight of the volume rendering visualisation for the weighted
     * averaging.
     * @param weight New weight value between 0 and 1
     */
    void
    onVolumeRenderingWeightChanged(double weight);

    /**
     * @brief Updates the UI state based on the selected target image fusion display node.
     */
    void
    updateFromMRMLImageFusionDisplayNode();

protected:
    /**
     * @brief Private implementation that holds all data
     */
    QScopedPointer<qSlicerImageFusionModuleWidgetPrivate> d_ptr;

    /**
     * @brief Sets up the UI representation. This is called when the module is opened for the first
     * time.
     */
    virtual void
    setup();

private:
    Q_DECLARE_PRIVATE(qSlicerImageFusionModuleWidget);
    Q_DISABLE_COPY(qSlicerImageFusionModuleWidget);
};
