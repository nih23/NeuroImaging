#pragma once

#include "vtkSlicerModuleLogic.h"

#include <cstdlib>

#include "vtkSlicerCameraCreatorModuleLogicExport.h"

class vtkMRMLNode;
class vtkMRMLCameraNode;
class vtkMRMLLinearTransformNode;

/**
 * @brief Logic of the Camera Creator module
 *
 * Performs all changes to the target camera node.
 */
class VTK_SLICER_CAMERACREATOR_MODULE_LOGIC_EXPORT vtkSlicerCameraCreatorLogic
    : public vtkSlicerModuleLogic {
public:
    /**
     * @brief Creates a new vtkSlicerCameraCreatorLogic.
     * @return New object
     */
    static vtkSlicerCameraCreatorLogic *
    New();

    vtkTypeMacro(vtkSlicerCameraCreatorLogic, vtkSlicerModuleLogic);


    /**
     * @brief Sets the target camera node.
     * @param node New camera node
     */
    void
    SetCameraNode(vtkMRMLNode *node);

    /**
     * @brief Sets the linear transform node that holds the transform from the reference adapter to
     * the anatomy.
     * @param node New linear transform node
     */
    void
    SetReferenceToRASTransformNode(vtkMRMLNode *node);

    /**
     * @brief Sets the linear transform node that holds the transform from the camera to the
     * reference adapter.
     * @param node New linear transform node
     */
    void
    SetCameraToReferenceTransformNode(vtkMRMLNode *node);

    /**
     * @brief Sets the view up vector in camera coordinates.
     * @param viewUp New 3D vector
     */
    void
    SetViewUp(double viewUp[3]);

    /**
     * @brief Sets the view direction vector in camera coordinates.
     * @param viewDirection New 3D vector
     */
    void
    SetViewDirection(double viewDirection[3]);

    /**
     * @brief Sets the vertical view angle in degrees.
     * @param viewAngle
     */
    void
    SetViewAngle(double viewAngle);

    /**
     * @brief Updates the target camera based on the current state of the transforms, the view up
     * vector, the view direction vector and the vertical camera view angle.
     */
    void
    UpdateCamera();

protected:
    /**
     * @brief Creates a new vtkSlicerCameraCreatorLogic.
     * @note Used by New()
     */
    vtkSlicerCameraCreatorLogic();

    /**
     * @brief Destroys the vtkSlicerCameraCreatorLogic.
     */
    virtual ~vtkSlicerCameraCreatorLogic();

    /**
     * @brief Sets a new MRMLScene and observes relevant node events.
     * @param newScene New scene
     */
    virtual void
    SetMRMLSceneInternal(vtkMRMLScene *newScene) VTK_OVERRIDE;

    /**
     * @brief Updates the module based on the current state of the MRML scene.
     */
    virtual void
    UpdateFromMRMLScene() VTK_OVERRIDE;

    /**
     * @brief Handles events of observed nodes.
     * @param caller Node that emitted the event
     * @param event Event type
     * @param callData Optional user data
     */
    virtual void
    ProcessMRMLNodesEvents(vtkObject *caller, unsigned long event, void *callData) VTK_OVERRIDE;

private:
    vtkSlicerCameraCreatorLogic(const vtkSlicerCameraCreatorLogic &); // Not implemented

    void
    operator=(const vtkSlicerCameraCreatorLogic &); // Not implemented

    /**
     * @brief Target camera
     */
    vtkMRMLCameraNode *CameraNode;

    /**
     * @brief Transform from the reference adapater coordinate system to the anatomy coordinate
     * system (RAS)
     */
    vtkMRMLLinearTransformNode *ReferenceToRASTransformNode;

    /**
     * @brief Transform from the camera coordinate system to the reference adapter coordinate
     * system
     */
    vtkMRMLLinearTransformNode *CameraToReferenceTransformNode;

    /**
     * @brief Camera view up vector in camera coordinates
     */
    double ViewUp[3];

    /**
     * @brief Camera view direction vector in camera coordinates
     */
    double ViewDirection[3];

    /**
     * @brief Vertical camera view angle in degrees
     */
    double ViewAngle;
};
