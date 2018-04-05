#pragma once

#include "vtkSlicerImageFusionModuleMRMLDisplayableManagerExport.h"

#include <vtkMRMLAbstractThreeDViewDisplayableManager.h>

class vtkPropCollection;
class vtkProp;
class vtkRenderWindow;

class vtkMRMLDisplayNode;
class vtkMRMLDisplayableNode;
class vtkMRMLImageFusionDisplayNode;
class vtkMRMLVolumeRenderingDisplayNode;
class vtkMRMLProjectiveTextureMappingDisplayNode;

/**
 * @brief Image fusion displayable manager
 *
 * Observes the scene for image fusion display nodes and creates according visualisations.
 */
class VTK_SLICER_IMAGEFUSION_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT
    vtkMRMLImageFusionDisplayableManager : public vtkMRMLAbstractThreeDViewDisplayableManager {
public:
    /**
     * @brief Creates a new vtkMRMLImageFusionDisplayableManager.
     * @return New object
     */
    static vtkMRMLImageFusionDisplayableManager *
    New();

    vtkTypeMacro(vtkMRMLImageFusionDisplayableManager, vtkMRMLAbstractThreeDViewDisplayableManager);

protected:
    /**
     * @brief Creates a new vtkMRMLImageFusionDisplayableManager.
     * @note Used by New()
     */
    vtkMRMLImageFusionDisplayableManager();

    /**
     * @brief Destroys the vtkMRMLImageFusionDisplayableManager.
     */
    virtual ~vtkMRMLImageFusionDisplayableManager();

    /**
     * @brief Callback that is used to observe a Renderer.
     * @param caller
     * @param eid
     * @param clientData
     * @param callData
     */
    static void
    RendererCallback(vtkObject *caller, unsigned long eid, void *clientData, void *callData);

    /**
     * @brief Sets a flag that indicates if the object is in the RendererCallback()
     * @param newFlag New flag
     */
    void
    SetInRendererCallbackFlag(int newFlag);

    /**
     * @brief Checks if the object is in the RendererCallback()
     * @return Status
     */
    int
    GetInRendererCallbackFlag();

    /**
     * @brief Sets a new Renderer and starts observing its render window.
     * @param newRenderer New renderer
     */
    virtual void
    SetRenderer(vtkRenderer *newRenderer) VTK_OVERRIDE;

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
    ProcessRendererEvents(vtkObject *caller, unsigned long event, void *callData);

    /**
     * @brief Handles node events
     * @param caller Node that emitted the event
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
     * @brief Removes a visualisation for the image fusion display node with the given identifier.
     * @param imageFusionDisplayID Identifier of the image fusion display node
     */
    void
    RemoveViewElementByID(std::string const &imageFusionDisplayID);

    /**
     * @brief Removes all visualisation that don't have a corresponding image fusion display node
     * anymore.
     */
    void
    RemoveInvalidViewElements();

    /**
     * @brief Removes all observations to image fusion display nodes
     */
    void
    RemoveDisplayNodeObservations();

    /**
     * @brief Adds initial visualisation objects for all new image fusion display nodes.
     */
    void
    AddNewViewElements();

    /**
     * @brief Updates all visualisation from their image fusion display nodes.
     */
    void
    UpdateViewElementsFromMRML();

    /**
     * @brief Checks if a MRML image fusion display node can be used for a visualisation.
     * @param node Image fusion display node
     * @return Status
     */
    bool
    IsDisplayNodeValid(vtkMRMLImageFusionDisplayNode *node) const;

    /**
     * @brief Checks if a volume rendering display node can be used for image fusion.
     * @param node Volume rendering display node
     * @return Status
     */
    bool
    IsDisplayNodeValid(vtkMRMLVolumeRenderingDisplayNode *node) const;

    /**
     * @brief Checks if a projective texture mapping display node can be used for image fusion.
     * @param node Projective texture mapping display node
     * @return
     */
    bool
    IsDisplayNodeValid(vtkMRMLProjectiveTextureMappingDisplayNode *node) const;

    /**
     * @brief Finds a matching Prop for a given volume rendering display node amongst all props.
     * @param node Volume rendering display node
     * @return Prop object
     */
    vtkProp *
    FindMatchingProp(vtkMRMLVolumeRenderingDisplayNode *node);

    /**
     * @brief Finds a matching Prop for a given projective texture mapping display node amongst all
     * props.
     * @param node Projective texture mapping display node
     * @return Prop object
     */
    vtkProp *
    FindMatchingProp(vtkMRMLProjectiveTextureMappingDisplayNode *node);

    /**
     * @brief Updates all input images that should be fused.
     */
    void
    UpdateInputImages();

private:
    vtkMRMLImageFusionDisplayableManager(const vtkMRMLImageFusionDisplayableManager &)
        VTK_DELETE_FUNCTION;

    void
    operator=(const vtkMRMLImageFusionDisplayableManager &) VTK_DELETE_FUNCTION;

    /**
     * Implementation specific class that manages all objects needed for an image fusion based
     * visualisation.
     */
    class vtkViewElement;

    /**
     * @brief Map from image fusion display node identifiers to visualisation objects
     */
    std::map<std::string, vtkViewElement *> ViewElements;

    /**
     * @brief Identifier of the currently shown/active image fusion display node
     */
    std::string ActiveDisplayNodeID;

    /**
     * @brief Cached pointer to the RendererProps that contains all props of the managed 3D view.
     */
    vtkPropCollection *RendererProps;

    /**
     * @brief Cached pointer to the RendererWindow that displays the  image fusion based
     * visualisation
     */
    vtkRenderWindow *RenderWindow;

    /**
     * @brief Observer manager that manages observations of Renderer events.
     */
    vtkObserverManager *RendererObserverManager;

    /**
     * @brief Flag that indicates if the object is in the RendererCallback()
     */
    int InRenderCallbackFlag;
};
