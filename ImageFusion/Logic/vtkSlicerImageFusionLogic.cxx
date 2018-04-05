#include "vtkSlicerImageFusionLogic.h"

#include <vtkMRMLScene.h>

#include "vtkMRMLImageFusionDisplayNode.h"

#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

#include <cassert>

vtkStandardNewMacro(vtkSlicerImageFusionLogic);

vtkSlicerImageFusionLogic::vtkSlicerImageFusionLogic() {}

vtkSlicerImageFusionLogic::~vtkSlicerImageFusionLogic() {}

void
vtkSlicerImageFusionLogic::RegisterNodes() {
    assert(this->GetMRMLScene() != 0);

    vtkNew<vtkMRMLImageFusionDisplayNode> imageFusionDisplayNode;
    this->GetMRMLScene()->RegisterNodeClass(imageFusionDisplayNode);
}
