#include "vtkSlicerProjectiveTextureMappingLogic.h"

#include <vtkMRMLScene.h>

#include "vtkMRMLProjectiveTextureMappingDisplayNode.h"

#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

#include <cassert>

vtkStandardNewMacro(vtkSlicerProjectiveTextureMappingLogic);

vtkSlicerProjectiveTextureMappingLogic::vtkSlicerProjectiveTextureMappingLogic() {}

vtkSlicerProjectiveTextureMappingLogic::~vtkSlicerProjectiveTextureMappingLogic() {}

void
vtkSlicerProjectiveTextureMappingLogic::RegisterNodes() {
    assert(this->GetMRMLScene() != 0);

    vtkNew<vtkMRMLProjectiveTextureMappingDisplayNode> ptmDisplayNode;
    this->GetMRMLScene()->RegisterNodeClass(ptmDisplayNode);
}
