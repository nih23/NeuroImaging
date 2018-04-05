#pragma once

#include "vtkSlicerProjectiveTextureMappingModuleMRMLDisplayableManagerExport.h"

#include <vtkMRMLAbstractThreeDViewDisplayableManager.h>

class vtkMRMLDisplayNode;
class vtkMRMLDisplayableNode;
class vtkMRMLProjectiveTextureMappingDisplayNode;

/**
 * @brief Projective texture mapping displayable manager
 *
 * Observes the scene for projective texture mapping display nodes and creates according
 * visualisations.
 */
class VTK_SLICER_PROJECTIVETEXTUREMAPPING_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT
    vtkMRMLProjectiveTextureMappingDisplayableManager
    : public vtkMRMLAbstractThreeDViewDisplayableManager {
public:
    /**
     * @brief Creates a new vtkMRMLProjectiveTextureMappingDisplayableManager.
     * @return New object
     */
    static vtkMRMLProjectiveTextureMappingDisplayableManager *
    New();

    vtkTypeMacro(vtkMRMLProjectiveTextureMappingDisplayableManager,
                 vtkMRMLAbstractThreeDViewDisplayableManager);

protected:
    /**
     * @brief Creates a new vtkMRMLProjectiveTextureMappingDisplayableManager.
     * @note Used by New()
     */
    vtkMRMLProjectiveTextureMappingDisplayableManager();

    /**
     * @brief Destroys the vtkMRMLProjectiveTextureMappingDisplayableManager.
     */
    virtual ~vtkMRMLProjectiveTextureMappingDisplayableManager();

    /**
     * @brief Removes all observations from the MRML scene.
     */
    virtual void
    UnobserveMRMLScene() VTK_OVERRIDE;

    /**
     * @brief Handles the event that the current MRML scene is closing
     */
    virtual void
    OnMRMLSceneStartClose() VTK_OVERRIDE;

    /**
     * @brief Handles the event that the current MRML scene was closed.
     */
    virtual void
    OnMRMLSceneEndClose() VTK_OVERRIDE;

    /**
     * @brief Requests a rerendering of all visualisations.
     * @note This is called when a batch process whas finished and when a new scene is set
     */
    virtual void
    UpdateFromMRMLScene() VTK_OVERRIDE;

    /**
     * @brief Handles the event that a new node is added to the MRML scene.
     * @note This calls UpdateFromMRML() if necessary
     * @param node Added node
     */
    virtual void
    OnMRMLSceneNodeAdded(vtkMRMLNode *node) VTK_OVERRIDE;

    /**
     * @brief Handles the event that a node was removed from the MRML scene.
     * @note This calls UpdateFromMRML() if necessary
     * @param node Removed node
     */
    virtual void
    OnMRMLSceneNodeRemoved(vtkMRMLNode *node) VTK_OVERRIDE;

    /**
     * @brief Handles renderer events
     * @param caller Caller that emitted the event
     * @param event Event type
     * @param callData User data
     */
    virtual void
    ProcessMRMLNodesEvents(vtkObject *caller, unsigned long event, void *callData) VTK_OVERRIDE;

    /**
     * @brief Updates all visualisiations based on the current state of the MRML scene.
     */
    virtual void
    UpdateFromMRML() VTK_OVERRIDE;

    /**
     * @brief Removes all MRML Observers.
     */
    virtual void
    RemoveMRMLObservers() VTK_OVERRIDE;

    /**
     * @brief Removes all visualisations.
     */
    void
    ClearViewElements();

    /**
     * @brief Removes a visualisation for the projective texture mapping display node with the given
     * identifier.
     * @param ptmDisplayID Identifier of the projective texture mapping display node
     */
    void
    RemoveViewElementByID(std::string const &ptmDisplayID);

    /**
     * @brief Removes all visualisation that don't have a corresponding projective texture mapping
     * display node anymore.
     */
    void
    RemoveInvalidViewElements();

    /**
     * @brief Removes all observations to projective texture mapping display nodes
     */
    void
    RemoveDisplayNodeObservations();

    /**
     * @brief Adds initial visualisation objects for all new projective texture mapping display nodes.
     */
    void
    AddNewViewElements();

    /**
     * @brief Updates all visualisation from their projective texture mapping display nodes.
     */
    void
    UpdateViewElementsFromMRML();

    /**
     * @brief Updates the depth image for a depth image for a given projective texture mapping
     * display node.
     * @param displayNode Projective texture mapping display node
     */
    void
    UpdateDepthBuffer(vtkMRMLProjectiveTextureMappingDisplayNode *displayNode);

    /**
     * @brief Checks if the visualisation of a given projective texture mapping display nodes needs
     * to be rendered again.
     * @param displayNode Projective texture mapping display node
     * @return Status
     */
    bool
    IsRerenderingRequired(vtkMRMLProjectiveTextureMappingDisplayNode *displayNode);

    /**
     * @brief Checks if a projective texture mapping display node can be used for visualisation.
     * @param node Projective texture mapping display node
     * @return
     */
    bool
    IsDisplayNodeValid(vtkMRMLProjectiveTextureMappingDisplayNode *node) const;

private:
    vtkMRMLProjectiveTextureMappingDisplayableManager(
        const vtkMRMLProjectiveTextureMappingDisplayableManager &) VTK_DELETE_FUNCTION;

    void
    operator=(const vtkMRMLProjectiveTextureMappingDisplayableManager &) VTK_DELETE_FUNCTION;

    /**
     * Implementation specific class that manages all objects needed for an projective texture
     * mapping based visualisation.
     */
    class vtkViewElement;

    /**
     * @brief Map from projective texture mapping display node identifiers to visualisation objects
     */
    std::map<std::string, vtkViewElement *> ViewElements;
};
