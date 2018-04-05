#pragma once

#include <vtkSlicerOpenIGTLinkIFLogic.h>

#include "vtkIGTLToMRMLImageWithFocus.h"

#include "vtkSlicerModuleLogic.h"

#include <cstdlib>
#include <memory>

#include "ImageWithFocusMessage.h"
#include "vtkSlicerImageWithFocusOpenIGTLinkModuleLogicExport.h"

/**
 * @brief Logic of the ImageWithFocusOpenIGTLinkLogic module
 *
 * Registers the message converter to the logic of the module OpenIGTLinkIF
 */
class VTK_SLICER_IMAGEWITHFOCUSOPENIGTLINK_MODULE_LOGIC_EXPORT
    vtkSlicerImageWithFocusOpenIGTLinkLogic : public vtkSlicerModuleLogic {
public:
    /**
     * @brief Creates a new vtkSlicerImageWithFocusOpenIGTLinkLogic.
     * @return New object
     */
    static vtkSlicerImageWithFocusOpenIGTLinkLogic *
    New();

    vtkTypeMacro(vtkSlicerImageWithFocusOpenIGTLinkLogic, vtkSlicerModuleLogic);

    /**
     * @brief Sets the OpenIGTLinkIF logic object
     * @param openIGTLinkIFLogic OpenIGTLinkIF logic object
     */
    void
    SetOpenIGTLinkIFLogic(vtkSlicerOpenIGTLinkIFLogic *openIGTLinkIFLogic);

protected:
    /**
     * @brief Creates a new vtkSlicerImageWithFocusOpenIGTLinkLogic.
     * @note Used by New()
     */
    vtkSlicerImageWithFocusOpenIGTLinkLogic();

    /**
     * @brief Destroys this vtkSlicerImageWithFocusOpenIGTLinkLogic.
     */
    virtual ~vtkSlicerImageWithFocusOpenIGTLinkLogic();

    /**
     * @brief Registers the OpenIGTLink converter for the custom image messages.
     */
    virtual void
    RegisterNodes() VTK_OVERRIDE;

private:
    vtkSlicerImageWithFocusOpenIGTLinkLogic(
        const vtkSlicerImageWithFocusOpenIGTLinkLogic &); // Not implemented

    void
    operator=(const vtkSlicerImageWithFocusOpenIGTLinkLogic &); // Not implemented

    /**
     * @brief Converter between MRML volumes and custom OpenIGTLink messages
     */
    vtkNew<vtkIGTLToMRMLImageWithFocus> IGTLToMRMLImageWithFocus;

    /**
     * @brief Cached pointer to the logic of the module OpenIGTLinkIF
     */
    vtkSlicerOpenIGTLinkIFLogic *OpenIGTLinkIFLogic;
};
