#pragma once

#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerProjectiveTextureMappingModuleExport.h"

class qSlicerProjectiveTextureMappingModuleWidgetPrivate;
class vtkMRMLNode;
class vtkMRMLModelNode;
class vtkMRMLVolumeNode;
class vtkMRMLProjectiveTextureMappingDisplayNode;
class vtkMRMLCameraNode;
class vtkMRMLColorNode;

/**
 * @brief UI representation of the Projective Texture Mapping module
 *
 * Hands over all inputs to the selected projective texture mapping display node.
 */
class Q_SLICER_QTMODULES_PROJECTIVETEXTUREMAPPING_EXPORT qSlicerProjectiveTextureMappingModuleWidget
    : public qSlicerAbstractModuleWidget {
    Q_OBJECT
    QVTK_OBJECT
public:
    typedef qSlicerAbstractModuleWidget Superclass;

    /**
     * @brief Creates a new qSlicerProjectiveTextureMappingModuleWidget.
     * @param parent Parent of the new object
     */
    qSlicerProjectiveTextureMappingModuleWidget(QWidget *parent = 0);

    /**
     * @brief Destroys the qSlicerProjectiveTextureMappingModuleWidget.
     */
    virtual ~qSlicerProjectiveTextureMappingModuleWidget();

    /**
     * @brief Set an edited node for the module. This can be used by other modules.
     * @param node Projective texture mapping display node
     * @param role Role of the node
     * @param context Context of the node
     * @return Status
     */
    virtual bool
    setEditedNode(vtkMRMLNode *node, QString role = QString(), QString context = QString());

    /**
     * @brief Gets a certainty that this module can edit a given node.
     * @note This module can edit projective texture mapping display nodes
     * @param node Checked node
     * @return Certainty value between 0 and 1
     */
    virtual double
    nodeEditable(vtkMRMLNode *node);

protected slots:
    /**
     * @brief Handles a selection change of the target projective texture mapping display node.
     * @param node New projective texture mapping display node
     */
    void
    onCurrentMRMLDisplayNodeChanged(vtkMRMLNode *node);

    /**
     * @brief Handles a selection change of the model node that contains the surface model.
     * @param node New model node
     */
    void
    onCurrentMRMLSurfaceModelNodeChanged(vtkMRMLNode *node);

    /**
     * @brief Handles a selection change of the volume node that contains the texture data.
     * @param node New volume node
     */
    void
    onCurrentMRMLTextureNodeChanged(vtkMRMLNode *node);

    /**
     * @brief Handles a selection change of the color map node that contains the color map which
     * maps texture values to colors
     * @param node New color map node
     */
    void
    onCurrentMRMLColorMapNodeChanged(vtkMRMLNode *node);

    /**
     * @brief Handles a selection change of the camera node that contains the virtual camera view.
     * @param node New camera node
     */
    void
    onCurrentMRMLCameraNodeChanged(vtkMRMLNode *node);

    /**
     * @brief Handles a change of the z tolerance.
     * @param zTolerance New z tolerance
     */
    void
    onZToleranceChanged(double zTolerance);

    /**
     * @brief Handles a change of the visibility.
     * @param visible New visibility.
     */
    void
    onVisibilityChanged(bool visible);

    /**
     * @brief Updates the UI state based on the selected target projective texture mapping display
     * node.
     */
    void
    updateFromMRMLProjectiveTextureMappingDisplayNode();

protected:
    /**
     * @brief Private implementation that holds all the data
     */
    QScopedPointer<qSlicerProjectiveTextureMappingModuleWidgetPrivate> d_ptr;

    /**
     * @brief Sets up the UI representation. This is called when the module is opened for the first
     * time.
     */
    virtual void
    setup();

private:
    Q_DECLARE_PRIVATE(qSlicerProjectiveTextureMappingModuleWidget);
    Q_DISABLE_COPY(qSlicerProjectiveTextureMappingModuleWidget);
};
