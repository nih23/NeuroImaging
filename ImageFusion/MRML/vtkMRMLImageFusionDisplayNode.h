#pragma once

#include <vtkMRMLDisplayNode.h>

#include "vtkSlicerImageFusionModuleMRMLExport.h"

class vtkMRMLVolumeRenderingDisplayNode;
class vtkMRMLProjectiveTextureMappingDisplayNode;

/**
 * @brief MRML image fusion display node
 *
 * Stores all data about an image fusion visualisiation
 */
class VTK_SLICER_IMAGEFUSION_MODULE_MRML_EXPORT vtkMRMLImageFusionDisplayNode : public vtkMRMLDisplayNode {
public:
    /**
     * @brief Creates a new vtkMRMLImageFusionDisplayNode.
     * @return New object
     */
    static vtkMRMLImageFusionDisplayNode *
    New();

    vtkTypeMacro(vtkMRMLImageFusionDisplayNode, vtkMRMLDisplayNode);

    /**
     * @brief Prints the state of this object.
     * @param os Output stream
     * @param indent Identation that is prefixed before all outputs
     */
    void
    PrintSelf(ostream &os, vtkIndent indent) VTK_OVERRIDE;

    /**
     * @brief Creates a new vtkMRMLImageFusionDisplayNode
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

    vtkGetStringMacro(VolumeRenderingDisplayNodeID);

    /**
     * @brief Sets and observes the fused volume rendering display node based on its identifier.
     * @param volumeRenderingDisplayNodeID Identifier of the volume rendering display node
     */
    void
    SetAndObserveVolumeRenderingDisplayNodeID(const char *volumeRenderingDisplayNodeID);

    /**
     * @brief Gets the fused volume rendering display node
     * @return Volume rendering display node object
     */
    vtkMRMLVolumeRenderingDisplayNode *
    GetVolumeRenderingDisplayNode();

    vtkGetStringMacro(ProjectiveTextureMappingDisplayNodeID);

    /**
     * @brief Sets and observes the fused projective texture mapping display node based on its
     * identifier.
     * @param volumeRenderingDisplayNodeID Identifier of the projective texture mapping display node
     */
    void
    SetAndObserveProjectiveTextureMappingDisplayNodeID(const char *projectiveTextureMappingDisplayNodeID);

    /**
     * @brief Gets the fused projective texture mapping display node.
     * @return Fused projective texture mapping display node object
     */
    vtkMRMLProjectiveTextureMappingDisplayNode *
    GetProjectiveTextureMappingDisplayNode();

    /**
     * @brief Possible image fusion methods
     */
    enum ImageFusionMethod { WEIGHTED_AVERAGING, GUIDED_FILTERING };

    vtkSetMacro(ImageFusionMethod, int);

    vtkGetMacro(ImageFusionMethod, int);

    /**
     * @brief Sets the used image fusion method to weighted averaging.
     */
    void
    SetImageFusionMethodToWeightedAveraging() {
        this->SetImageFusionMethod(WEIGHTED_AVERAGING);
    }

    /**
     * @brief Sets the used image fusion method to guided filtering.
     */
    void
    SetImageFusionMethodToGuidedFiltering() {
        this->SetImageFusionMethod(GUIDED_FILTERING);
    }

    vtkSetClampMacro(VolumeRenderingWeight, double, 0.0, 1.0);

    vtkGetMacro(VolumeRenderingWeight, double);

protected:
    /**
     * @brief Creates a new vtkMRMLImageFusionDisplayNode.
     * @note Used by New()
     */
    vtkMRMLImageFusionDisplayNode();

    /**
     * @brief Destroys the vtkMRMLImageFusionDisplayNode.
     */
    virtual ~vtkMRMLImageFusionDisplayNode();

    vtkMRMLImageFusionDisplayNode(const vtkMRMLImageFusionDisplayNode &); // not implemented

    void
    operator=(const vtkMRMLImageFusionDisplayNode &); // not implemented

    /**
     * @brief Handles observed node events
     * @param caller Node that emitted the event
     * @param event Event type
     * @param callData User data
     */
    virtual void
    ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData) VTK_OVERRIDE;

    /**
     * @brief Sets the identifier of the fused volume rendering display node.
     * @param arg Identifier
     */
    void
    SetVolumeRenderingDisplayNodeID(const char *arg);

    /**
     * @brief Identifier of the fused volume rendering display node
     */
    char *VolumeRenderingDisplayNodeID;

    /**
     * @brief Fused volume rendering display node
     */
    vtkMRMLVolumeRenderingDisplayNode *VolumeRenderingDisplayNode;

    /**
     * @brief Sets the identifier of the fused volume rendering display node.
     * @param arg Identifier
     */
    void
    SetProjectiveTextureMappingDisplayNodeID(const char *arg);

    /**
     * @brief Identifier of the fused projective texture mapping display node
     */
    char *ProjectiveTextureMappingDisplayNodeID;

    /**
     * @brief Fused projective texture mapping display node
     */
    vtkMRMLProjectiveTextureMappingDisplayNode *ProjectiveTextureMappingDisplayNode;

    /**
     * @brief Used image fusion method
     */
    int ImageFusionMethod;

    /**
     * @brief Weight of the volume rendering visualisation for the weighted averaging
     */
    double VolumeRenderingWeight;
};
