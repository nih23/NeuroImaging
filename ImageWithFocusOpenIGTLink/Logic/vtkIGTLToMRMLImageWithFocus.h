#pragma once

#include <vtkIGTLToMRMLImage.h>

#include "ImageWithFocusMessage.h"

#include "vtkSlicerImageWithFocusOpenIGTLinkModuleLogicExport.h"

/**
 * @brief Converts the custom OpenIGTLink image messages to MRML volume nodes.
 */
class VTK_SLICER_IMAGEWITHFOCUSOPENIGTLINK_MODULE_LOGIC_EXPORT vtkIGTLToMRMLImageWithFocus
    : public vtkIGTLToMRMLImage {
public:
    /**
     * @brief Creates a new vtkIGTLToMRMLImageWithFocus
     * @return New object
     */
    static vtkIGTLToMRMLImageWithFocus *
    New();

    vtkTypeMacro(vtkIGTLToMRMLImageWithFocus, vtkIGTLToMRMLImage);

    /**
     * @brief Gets the Message name / Device type that is handled from this converter
     * @return Message name / Device type
     */
    virtual const char *
    GetIGTLName() VTK_OVERRIDE {
        return "MYIMAGE";
    };

    /**
     * @brief Creates a new node for a message
     * @param scene MRML scene
     * @param name Name of the node
     * @param incomingImageMessage Incoming message
     * @return
     */
    virtual vtkMRMLNode *
    CreateNewNodeWithMessage(vtkMRMLScene *scene,
                             const char *name,
                             igtl::MessageBase::Pointer incomingImageMessage) VTK_OVERRIDE;

    /**
     * @brief Unpacks a given OpenIGTLink message.
     * @param buffer Message buffer
     * @return
     */
    virtual int
    UnpackIGTLMessage(igtl::MessageBase::Pointer buffer) VTK_OVERRIDE;

protected:
    vtkIGTLToMRMLImageWithFocus();
    ~vtkIGTLToMRMLImageWithFocus();

protected:
    igtl::ImageWithFocusMessage::Pointer InImageWithFocusMessage;
};
