#pragma once

#include "vtkSlicerModuleLogic.h"

#include <cstdlib>

#include "vtkSlicerImageFusionModuleLogicExport.h"

/**
 * @brief Logic of the Image Fusion module
 *
 * Registers the image fusion display node class.
 */
class VTK_SLICER_IMAGEFUSION_MODULE_LOGIC_EXPORT vtkSlicerImageFusionLogic
    : public vtkSlicerModuleLogic {
public:
    /**
     * @brief Creates a new vtkSlicerImageFusionLogic.
     * @return New object
     */
    static vtkSlicerImageFusionLogic *
    New();

    vtkTypeMacro(vtkSlicerImageFusionLogic, vtkSlicerModuleLogic);

protected:
    /**
     * @brief Creates a new vtkSlicerImageFusionLogic.
     * @note Used by New()
     */
    vtkSlicerImageFusionLogic();


    /**
     * @brief Destroys the vtkSlicerImageFusionLogic.
     */
    virtual ~vtkSlicerImageFusionLogic();

    /**
     * @brief Registers the vtkMRMLImageFusionDisplayNode class to the MRML scene.
     */
    virtual void
    RegisterNodes() VTK_OVERRIDE;

private:
    vtkSlicerImageFusionLogic(const vtkSlicerImageFusionLogic &); // Not implemented

    void
    operator=(const vtkSlicerImageFusionLogic &); // Not implemented
};
