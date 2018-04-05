#pragma once

#include "vtkSlicerModuleLogic.h"

#include <cstdlib>

#include "vtkSlicerProjectiveTextureMappingModuleLogicExport.h"

#include "vtkMRMLProjectiveTextureMappingDisplayNode.h"

/**
 * @brief Logic of the Projective Texture Mapping module
 *
 * Registers the projective texture mapping display node class.
 */
class VTK_SLICER_PROJECTIVETEXTUREMAPPING_MODULE_LOGIC_EXPORT vtkSlicerProjectiveTextureMappingLogic
    : public vtkSlicerModuleLogic {
public:
    /**
     * @brief Creates a new vtkSlicerProjectiveTextureMappingLogic.+
     * @return New object
     */
    static vtkSlicerProjectiveTextureMappingLogic *
    New();

    vtkTypeMacro(vtkSlicerProjectiveTextureMappingLogic, vtkSlicerModuleLogic);

protected:
    /**
     * @brief Create a new vtkSlicerProjectiveTextureMappingLogic.
     * @note Used by New()
     */
    vtkSlicerProjectiveTextureMappingLogic();

    /**
     * @brief Destroys the vtkSlicerProjectiveTextureMappingLogic.
     */
    virtual ~vtkSlicerProjectiveTextureMappingLogic();

    /**
     * @brief Registers the vtkMRMLProjectiveTextureMappingDisplayNode class to the MRML scene.
     */
    virtual void
    RegisterNodes() VTK_OVERRIDE;

private:
    vtkSlicerProjectiveTextureMappingLogic(
        const vtkSlicerProjectiveTextureMappingLogic &); // Not implemented

    void
    operator=(const vtkSlicerProjectiveTextureMappingLogic &); // Not implemented
};
