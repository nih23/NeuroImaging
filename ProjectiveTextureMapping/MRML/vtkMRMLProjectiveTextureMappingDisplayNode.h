#pragma once

#include <vtkMRMLDisplayNode.h>

class vtkMRMLModelNode;
class vtkMRMLVolumeNode;
class vtkMRMLCameraNode;
class vtkMRMLColorNode;

/**
 * @brief MRML projective texture mapping display node
 *
 * Stores all data about an projective texture mapping visualisation.
 */
class vtkMRMLProjectiveTextureMappingDisplayNode : public vtkMRMLDisplayNode {
public:
    /**
     * @brief Possible event types that are proxied from the referenced nodes
     * @note Event forwarding requires less observations for the displayable manager
     */
    enum {
        SurfaceModelDataModifiedEvent = 424242,
        TextureDataModifiedEvent,
        ColorMapModifiedEvent,
        CameraModifiedEvent
    };

    /**
     * @brief Creates a new vtkMRMLProjectiveTextureMappingDisplayNode.
     * @return New object
     */
    static vtkMRMLProjectiveTextureMappingDisplayNode *
    New();

    vtkTypeMacro(vtkMRMLProjectiveTextureMappingDisplayNode, vtkMRMLDisplayNode);

    /**
     * @brief Prints the state of this object.
     * @param os Output stream
     * @param indent Identation that is prefixed before all outputs
     */
    void
    PrintSelf(ostream &os, vtkIndent indent) VTK_OVERRIDE;

    /**
     * @brief Creates a new vtkMRMLProjectiveTextureMappingDisplayNode.
     * @return New object
     */
    virtual vtkMRMLNode *
    CreateNodeInstance() VTK_OVERRIDE;

    /**
     * @brief Updates the state of the node from an XML serialisation.
     * @param atts XML attributes (nullptr terminated string list)
     */
    virtual void
    ReadXMLAttributes(const char **atts) VTK_OVERRIDE;

    /**
     * @brief Serialises the state of the node to XML.
     * @param of Output stream
     * @param indent Indentation that is prefix to all outputs
     */
    virtual void
    WriteXML(ostream &of, int indent) VTK_OVERRIDE;

    /**
     * @brief Copies the state of this node to another node.
     * @param node Destination node
     */
    virtual void
    Copy(vtkMRMLNode *node) VTK_OVERRIDE;

    /**
     * @brief Sets the references to the observed nodes.
     */
    virtual void
    SetSceneReferences() VTK_OVERRIDE;

    /**
     * @brief Updates the identifier of a referenced node.
     * @param oldID Old identifier
     * @param newID New identifier
     */
    virtual void
    UpdateReferenceID(const char *oldID, const char *newID) VTK_OVERRIDE;

    /**
     * @brief Updates the references based on the current state of the scene.
     */
    virtual void
    UpdateReferences() VTK_OVERRIDE;

    /**
     * @brief Updates the MRML Scene.
     * @param scene New scene
     */
    virtual void
    UpdateScene(vtkMRMLScene *scene) VTK_OVERRIDE;

    /**
     * @brief Gets the unique node tag name for this class.
     * @return Tag name
     */
    virtual const char *
    GetNodeTagName() VTK_OVERRIDE;

    vtkGetStringMacro(SurfaceModelNodeID);

    /**
     * @brief Sets and observers the surface model node based on its identifier.
     * @param surfaceModelNodeID Identifier of the modeln node
     */
    void
    SetAndObserveSurfaceModelNodeID(const char *surfaceModelNodeID);

    /**
     * @brief Gets the surface model node.
     * @return Model node object
     */
    vtkMRMLModelNode *
    GetSurfaceModelNode();

    vtkGetStringMacro(TextureNodeID);

    /**
     * @brief Sets and observeres the projected texture node by its identifer.
     * @param textureNodeID Identifier of the volume node
     */
    void
    SetAndObserveTextureNodeID(const char *textureNodeID);

    /**
     * @brief Gets the projected texture node.
     * @return Volume node object
     */
    vtkMRMLVolumeNode *
    GetTextureNode();

    vtkGetStringMacro(ColorMapNodeID);

    /**
     * @brief Sets and observes the color map node by its idenitifer.
     * @param colorMapNodeID Identifier of the color map node
     */
    void
    SetAndObserveColorMapNodeID(const char *colorMapNodeID);

    /**
     * @brief Gets the used color map node.
     * @return Color node object
     */
    vtkMRMLColorNode *
    GetColorMapNode();

    vtkGetStringMacro(CameraNodeID);

    /**
     * @brief Sets and observes the camera node by its identifier.
     * @param cameraNodeID Identifier of the camera node
     */
    void
    SetAndObserveCameraNodeID(const char *cameraNodeID);

    /**
     * @brief Gets the camera node.
     * @return Camer node object
     */
    vtkMRMLCameraNode *
    GetCameraNode();

    vtkSetClampMacro(ZTolerance, double, 0.0, 1.0);
    vtkGetMacro(ZTolerance, double);

protected:
    /**
     * @brief Creates a new vtkMRMLProjectiveTextureMappingDisplayNode.
     * @note Used by New()
     */
    vtkMRMLProjectiveTextureMappingDisplayNode();

    /**
     * @brief Destroys the vtkMRMLProjectiveTextureMappingDisplayNode.
     */
    virtual ~vtkMRMLProjectiveTextureMappingDisplayNode();

    vtkMRMLProjectiveTextureMappingDisplayNode(
        const vtkMRMLProjectiveTextureMappingDisplayNode &); // not implemented

    void
    operator=(const vtkMRMLProjectiveTextureMappingDisplayNode &); // not implemented

    /**
     * @brief Handles observed node events
     * @param caller Node that emitted the event
     * @param event Event type
     * @param callData User data
     */
    virtual void
    ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData) VTK_OVERRIDE;

    /**
     * @brief Sets the identifier of the surface model node.
     * @param arg Identifier
     */
    void
    SetSurfaceModelNodeID(const char *arg);

    /**
     * @brief Identifier of the surface model node
     */
    char *SurfaceModelNodeID;

    /**
     * @brief Surface model node
     */
    vtkMRMLModelNode *SurfaceModelNode;

    /**
     * @brief Sets identifier of the projected texture node.
     * @param arg Identifier
     */
    void
    SetTextureNodeID(const char *arg);

    /**
     * @brief Projected texture node identifier
     */
    char *TextureNodeID;

    /**
     * @brief Projected texture node
     */
    vtkMRMLVolumeNode *TextureNode;

    /**
     * @brief Sets identifier of the color map node.
     * @param arg Identifier
     */
    void
    SetColorMapNodeID(const char *arg);

    /**
     * @brief Color map node identifier (optional)
     */
    char *ColorMapNodeID;

    /**
     * @brief Color map node
     */
    vtkMRMLColorNode *ColorMapNode;

    /**
     * @brief Sets identifier of the camera node.
     * @param arg Identifier
     */
    void
    SetCameraNodeID(const char *arg);

    /**
     * @brief Camera node identifier
     */
    char *CameraNodeID;

    /**
     * @brief Camera node
     */
    vtkMRMLCameraNode *CameraNode;

    /**
     * @brief z tolerance expressed as a fraction (0 to 1) of the diagonal of the bounding box of
     * the surface model
     */
    double ZTolerance;
};
