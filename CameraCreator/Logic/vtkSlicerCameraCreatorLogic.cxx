#include "vtkSlicerCameraCreatorLogic.h"

#include <vtkMRMLCameraNode.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLScene.h>

#include <vtkCamera.h>
#include <vtkIntArray.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

#include <cassert>

vtkStandardNewMacro(vtkSlicerCameraCreatorLogic);

void
vtkSlicerCameraCreatorLogic::SetCameraNode(vtkMRMLNode *node) {
    if (this->CameraNode) {
        this->CameraNode->UnRegister(this);
    }
    this->CameraNode = vtkMRMLCameraNode::SafeDownCast(node);
    if (this->CameraNode) {
        this->CameraNode->Register(this);
    }
}

void
vtkSlicerCameraCreatorLogic::SetReferenceToRASTransformNode(vtkMRMLNode *node) {
    if (node == this->ReferenceToRASTransformNode) {
        return;
    }

    vtkNew<vtkIntArray> events;
    events->InsertNextValue(vtkCommand::ModifiedEvent);
    events->InsertNextValue(vtkMRMLTransformableNode::TransformModifiedEvent);
    auto transformNode = vtkMRMLLinearTransformNode::SafeDownCast(node);
    vtkSetAndObserveMRMLNodeEventsMacro(this->ReferenceToRASTransformNode, transformNode, events);
}

void
vtkSlicerCameraCreatorLogic::SetCameraToReferenceTransformNode(vtkMRMLNode *node) {
    if (node == this->CameraToReferenceTransformNode) {
        return;
    }

    vtkNew<vtkIntArray> events;
    events->InsertNextValue(vtkCommand::ModifiedEvent);
    events->InsertNextValue(vtkMRMLTransformableNode::TransformModifiedEvent);
    auto transformNode = vtkMRMLLinearTransformNode::SafeDownCast(node);
    vtkSetAndObserveMRMLNodeEventsMacro(
        this->CameraToReferenceTransformNode, transformNode, events);
}

void
vtkSlicerCameraCreatorLogic::SetViewUp(double viewUp[]) {
    for (int i = 0; i < 3; ++i) {
        this->ViewUp[i] = viewUp[i];
    }
}

void
vtkSlicerCameraCreatorLogic::SetViewDirection(double viewDirection[]) {
    for (int i = 0; i < 3; ++i) {
        this->ViewDirection[i] = viewDirection[i];
    }
}

void
vtkSlicerCameraCreatorLogic::SetViewAngle(double viewAngle) {
    this->ViewAngle = viewAngle;
}

void
vtkSlicerCameraCreatorLogic::UpdateCamera() {
    if (this->CameraNode == nullptr || this->ReferenceToRASTransformNode == nullptr ||
        this->CameraToReferenceTransformNode == nullptr) {
        return;
    }

    double viewUpCamera[4], viewDirectionCamera[4], positionCamera[4];
    for (int i = 0; i < 3; ++i) {
        viewUpCamera[i] = this->ViewUp[i];
        viewDirectionCamera[i] = this->ViewDirection[i];
        positionCamera[i] = 0.0;
    }
    viewUpCamera[3] = viewDirectionCamera[3] = 0.0;
    positionCamera[3] = 1.0;

    double viewUpLength = vtkMath::Normalize(viewUpCamera);
    double viewDirectionLength = vtkMath::Normalize(viewDirectionCamera);
    if (viewUpLength <= 0.0 || viewDirectionLength <= 0.0 || this->ViewAngle < 0.0 ||
        this->ViewAngle > 180.0) {
        cerr << "Camera Creator: Input value was invalid" << endl;
        return;
    }

    vtkNew<vtkMatrix4x4> referenceToRAS;
    vtkNew<vtkMatrix4x4> cameraToReference;
    if (!this->ReferenceToRASTransformNode->GetMatrixTransformToParent(referenceToRAS) ||
        !this->CameraToReferenceTransformNode->GetMatrixTransformToParent(cameraToReference)) {
        cerr << "Camera Creator: Couldn't find a transform matrix" << endl;
        return;
    }

    vtkNew<vtkMatrix4x4> cameraToRAS;
    vtkMatrix4x4::Multiply4x4(referenceToRAS, cameraToReference, cameraToRAS);
    cout << "camera to RAS transform: " << endl;
    cameraToRAS->Print(cout);

    double viewUpRAS[4], viewDirectionRAS[4], positionRAS[4];
    cameraToRAS->MultiplyPoint(viewUpCamera, viewUpRAS);
    cameraToRAS->MultiplyPoint(viewDirectionCamera, viewDirectionRAS);
    cameraToRAS->MultiplyPoint(positionCamera, positionRAS);

    double focalPointRAS[3];
    for (int i = 0; i < 3; ++i) {
        focalPointRAS[i] = positionRAS[i] + viewDirectionRAS[i];
    }

    int modifyingDisabled = this->CameraNode->StartModify();

    vtkCamera *camera = this->CameraNode->GetCamera();
    camera->SetPosition(positionRAS);
    camera->SetViewUp(viewUpRAS);
    camera->SetFocalPoint(focalPointRAS);
    camera->SetViewAngle(this->ViewAngle);
    cout << "Camera modified" << endl;
    camera->Print(cout);

    this->CameraNode->EndModify(modifyingDisabled);
}

vtkSlicerCameraCreatorLogic::vtkSlicerCameraCreatorLogic() {
    this->CameraNode = nullptr;
    this->ReferenceToRASTransformNode = nullptr;
    this->CameraToReferenceTransformNode = nullptr;
    this->ViewUp[0] = this->ViewUp[1] = this->ViewUp[2] = 0.0;
    this->ViewDirection[0] = this->ViewDirection[1] = this->ViewDirection[2] = 0.0;
    this->ViewAngle = 0.0;
}

vtkSlicerCameraCreatorLogic::~vtkSlicerCameraCreatorLogic() {
    if (this->CameraNode) {
        this->CameraNode->UnRegister(this);
        this->CameraNode = nullptr;
    }
    vtkSetMRMLNodeMacro(this->ReferenceToRASTransformNode, nullptr);
    vtkSetMRMLNodeMacro(this->CameraToReferenceTransformNode, nullptr);
}

void
vtkSlicerCameraCreatorLogic::SetMRMLSceneInternal(vtkMRMLScene *newScene) {
    vtkNew<vtkIntArray> events;
    events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
    events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
    events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
    this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

void
vtkSlicerCameraCreatorLogic::UpdateFromMRMLScene() {
    assert(this->GetMRMLScene() != 0);
    this->Superclass::UpdateFromMRMLScene();
    vtkMRMLScene *scene = this->GetMRMLScene();
    if (this->CameraNode) {
        this->SetCameraNode(scene->GetNodeByID(this->CameraNode->GetID()));
    }
    if (this->ReferenceToRASTransformNode) {
        this->SetReferenceToRASTransformNode(
            scene->GetNodeByID(this->ReferenceToRASTransformNode->GetID()));
    }
    if (this->CameraToReferenceTransformNode) {
        this->SetCameraToReferenceTransformNode(
            scene->GetNodeByID(this->CameraToReferenceTransformNode->GetID()));
    }
    this->UpdateCamera();
}

void
vtkSlicerCameraCreatorLogic::ProcessMRMLNodesEvents(vtkObject *caller,
                                                    unsigned long event,
                                                    void *callData) {
    if (caller == this->ReferenceToRASTransformNode ||
        caller == this->CameraToReferenceTransformNode) {
        this->UpdateCamera();
    } else {
        this->Superclass::ProcessMRMLNodesEvents(caller, event, callData);
    }
}
