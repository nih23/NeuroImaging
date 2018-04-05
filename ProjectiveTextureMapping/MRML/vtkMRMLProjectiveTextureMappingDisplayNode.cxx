#include "vtkMRMLProjectiveTextureMappingDisplayNode.h"

#include <vtkMRMLCameraNode.h>
#include <vtkMRMLColorNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLVectorVolumeNode.h>

#include <vtkCommand.h>

#include <sstream>

vtkMRMLNodeNewMacro(vtkMRMLProjectiveTextureMappingDisplayNode);

vtkCxxSetReferenceStringMacro(vtkMRMLProjectiveTextureMappingDisplayNode, SurfaceModelNodeID);
vtkCxxSetReferenceStringMacro(vtkMRMLProjectiveTextureMappingDisplayNode, TextureNodeID);
vtkCxxSetReferenceStringMacro(vtkMRMLProjectiveTextureMappingDisplayNode, ColorMapNodeID);
vtkCxxSetReferenceStringMacro(vtkMRMLProjectiveTextureMappingDisplayNode, CameraNodeID);

void
vtkMRMLProjectiveTextureMappingDisplayNode::PrintSelf(std::ostream &os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);

    os << "SurfaceModelNodeID: " << (this->SurfaceModelNodeID ? this->SurfaceModelNodeID : "None")
       << "\n";
    os << "TextureNodeID: " << (this->TextureNodeID ? this->TextureNodeID : "None") << "\n";
    os << "ColorMapNodeID: " << (this->ColorMapNodeID ? this->ColorMapNodeID : "None") << "\n";
    os << "CameraNodeID: " << (this->CameraNodeID ? this->CameraNodeID : "None") << "\n";
    os << "ZTolerance: " << this->ZTolerance << "\n";
}

void
vtkMRMLProjectiveTextureMappingDisplayNode::ReadXMLAttributes(const char **atts) {
    this->Superclass::ReadXMLAttributes(atts);

    const char *attName;
    const char *attValue;
    while (*atts != nullptr) {
        attName = *(atts++);
        attValue = *(atts++);
        if (!strcmp(attName, "surfaceModelNodeID")) {
            this->SetSurfaceModelNodeID(attValue);
        } else if (!strcmp(attName, "textureNodeID")) {
            this->SetTextureNodeID(attValue);
        } else if (!strcmp(attName, "colorMapNodeID")) {
            this->SetColorMapNodeID(attValue);
        } else if (!strcmp(attName, "cameraNodeID")) {
            this->SetCameraNodeID(attValue);
        } else if (!strcmp(attName, "zTolerance")) {
            std::stringstream sstream{attValue};
            double zTolerance = 0.0;
            sstream >> zTolerance;
            this->SetZTolerance(zTolerance);
        }
    }
}

void
vtkMRMLProjectiveTextureMappingDisplayNode::WriteXML(std::ostream &of, int indent) {
    this->Superclass::WriteXML(of, indent);
    of << " surfaceModelNodeID=\"" << (this->SurfaceModelNodeID ? this->SurfaceModelNodeID : "NULL")
       << "\"";
    of << " textureNodeID=\"" << (this->TextureNodeID ? this->TextureNodeID : "NULL") << "\"";
    of << " colorMapNodeID=\"" << (this->ColorMapNodeID ? this->ColorMapNodeID : "NULL") << "\"";
    of << " cameraNodeID=\"" << (this->CameraNodeID ? this->CameraNodeID : "NULL") << "\"";
    of << " zTolerance=\"" << this->ZTolerance << "\"";
}

void
vtkMRMLProjectiveTextureMappingDisplayNode::Copy(vtkMRMLNode *anode) {
    int wasModifying = this->StartModify();
    this->Superclass::Copy(anode);
    auto node = vtkMRMLProjectiveTextureMappingDisplayNode::SafeDownCast(anode);

    this->SetSurfaceModelNodeID(node->GetSurfaceModelNodeID());
    this->SetTextureNodeID(node->GetTextureNodeID());
    this->SetColorMapNodeID(node->GetColorMapNodeID());
    this->SetCameraNodeID(node->GetCameraNodeID());
    this->SetZTolerance(node->GetZTolerance());

    this->EndModify(wasModifying);
}

void
vtkMRMLProjectiveTextureMappingDisplayNode::SetSceneReferences() {
    this->Superclass::SetSceneReferences();

    this->Scene->AddReferencedNodeID(this->SurfaceModelNodeID, this);
    this->Scene->AddReferencedNodeID(this->TextureNodeID, this);
    this->Scene->AddReferencedNodeID(this->ColorMapNodeID, this);
    this->Scene->AddReferencedNodeID(this->CameraNodeID, this);
}

void
vtkMRMLProjectiveTextureMappingDisplayNode::UpdateReferenceID(const char *oldID,
                                                              const char *newID) {
    this->Superclass::UpdateReferenceID(oldID, newID);

    if (this->SurfaceModelNodeID && !strcmp(oldID, this->SurfaceModelNodeID)) {
        this->SetAndObserveSurfaceModelNodeID(newID);
    } else if (this->TextureNodeID && !strcmp(oldID, this->TextureNodeID)) {
        this->SetAndObserveTextureNodeID(newID);
    } else if (this->ColorMapNodeID && !strcmp(oldID, this->ColorMapNodeID)) {
        this->SetAndObserveColorMapNodeID(newID);
    } else if (this->CameraNodeID && !strcmp(oldID, this->CameraNodeID)) {
        this->SetAndObserveCameraNodeID(newID);
    }
}

void
vtkMRMLProjectiveTextureMappingDisplayNode::UpdateReferences() {
    this->Superclass::UpdateReferences();

    if (this->SurfaceModelNodeID != nullptr &&
        this->Scene->GetNodeByID(this->SurfaceModelNodeID) == nullptr) {
        this->SetAndObserveSurfaceModelNodeID(nullptr);
    }
    if (this->TextureNodeID != nullptr &&
        this->Scene->GetNodeByID(this->TextureNodeID) == nullptr) {
        this->SetAndObserveTextureNodeID(nullptr);
    }
    if (this->ColorMapNodeID != nullptr &&
        this->Scene->GetNodeByID(this->ColorMapNodeID) == nullptr) {
        this->SetAndObserveColorMapNodeID(nullptr);
    }
    if (this->CameraNodeID != nullptr && this->Scene->GetNodeByID(this->CameraNodeID) == nullptr) {
        this->SetAndObserveCameraNodeID(nullptr);
    }
}

void
vtkMRMLProjectiveTextureMappingDisplayNode::UpdateScene(vtkMRMLScene *scene) {
    this->Superclass::UpdateScene(scene);

    this->SetAndObserveSurfaceModelNodeID(this->SurfaceModelNodeID);
    this->SetAndObserveTextureNodeID(this->TextureNodeID);
    this->SetAndObserveColorMapNodeID(this->ColorMapNodeID);
    this->SetAndObserveCameraNodeID(this->CameraNodeID);
}

const char *
vtkMRMLProjectiveTextureMappingDisplayNode::GetNodeTagName() {
    return "ProjectiveTextureMapping";
}

void
vtkMRMLProjectiveTextureMappingDisplayNode::SetAndObserveSurfaceModelNodeID(
    const char *surfaceModelNodeID) {
    vtkSetAndObserveMRMLObjectMacro(this->SurfaceModelNode, nullptr);

    this->SetSurfaceModelNodeID(surfaceModelNodeID);
    this->GetSurfaceModelNode();
}

vtkMRMLModelNode *
vtkMRMLProjectiveTextureMappingDisplayNode::GetSurfaceModelNode() {
    if (this->SurfaceModelNodeID == nullptr) {
        vtkSetAndObserveMRMLObjectMacro(this->SurfaceModelNode, nullptr);
    }
    if (this->GetScene() &&
        ((this->SurfaceModelNode != nullptr && this->SurfaceModelNodeID &&
          strcmp(this->SurfaceModelNode->GetID(), this->SurfaceModelNodeID)) ||
         this->SurfaceModelNode == nullptr)) {
        vtkMRMLNode *node = this->GetScene()->GetNodeByID(this->SurfaceModelNodeID);
        vtkNew<vtkIntArray> events;
        events->InsertNextValue(vtkCommand::ModifiedEvent);
        events->InsertNextValue(vtkMRMLTransformableNode::TransformModifiedEvent);
        events->InsertNextValue(vtkMRMLModelNode::MeshModifiedEvent);
        vtkSetAndObserveMRMLObjectEventsMacro(
            this->SurfaceModelNode, vtkMRMLModelNode::SafeDownCast(node), events);
    }
    return this->SurfaceModelNode;
}

void
vtkMRMLProjectiveTextureMappingDisplayNode::SetAndObserveTextureNodeID(const char *textureNodeID) {
    vtkSetAndObserveMRMLObjectMacro(this->TextureNode, nullptr);

    this->SetTextureNodeID(textureNodeID);
    this->GetTextureNode();
}

vtkMRMLVolumeNode *
vtkMRMLProjectiveTextureMappingDisplayNode::GetTextureNode() {
    if (this->TextureNodeID == nullptr) {
        vtkSetAndObserveMRMLObjectMacro(this->TextureNode, nullptr);
    } else if (this->GetScene() &&
               ((this->TextureNode != nullptr &&
                 strcmp(this->TextureNode->GetID(), this->TextureNodeID)) ||
                this->TextureNode == nullptr)) {
        vtkMRMLNode *node = this->GetScene()->GetNodeByID(this->TextureNodeID);
        vtkMRMLVolumeNode *volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(node);
        if (!volumeNode) {
            volumeNode = vtkMRMLVectorVolumeNode::SafeDownCast(node);
        }
        vtkNew<vtkIntArray> events;
        events->InsertNextValue(vtkCommand::ModifiedEvent);
        events->InsertNextValue(vtkMRMLTransformableNode::TransformModifiedEvent);
        events->InsertNextValue(vtkMRMLVolumeNode::ImageDataModifiedEvent);
        vtkSetAndObserveMRMLObjectEventsMacro(this->TextureNode, volumeNode, events);
    }
    return this->TextureNode;
}

void
vtkMRMLProjectiveTextureMappingDisplayNode::SetAndObserveColorMapNodeID(const char *colorMapNodeID) {
    vtkSetAndObserveMRMLObjectMacro(this->ColorMapNode, nullptr);

    this->SetColorMapNodeID(colorMapNodeID);
    this->GetColorMapNode();
}

vtkMRMLColorNode *
vtkMRMLProjectiveTextureMappingDisplayNode::GetColorMapNode() {
    if (this->ColorMapNodeID == nullptr) {
        vtkSetAndObserveMRMLObjectMacro(this->ColorMapNode, nullptr);
    } else if (this->GetScene() &&
               ((this->ColorMapNode != nullptr &&
                 strcmp(this->ColorMapNode->GetID(), this->ColorMapNodeID)) ||
                (this->ColorMapNode == nullptr))) {
        vtkMRMLNode *node = this->GetScene()->GetNodeByID(this->ColorMapNodeID);
        vtkSetAndObserveMRMLObjectMacro(this->ColorMapNode, vtkMRMLColorNode::SafeDownCast(node));
    }
    return this->ColorMapNode;
}

void
vtkMRMLProjectiveTextureMappingDisplayNode::SetAndObserveCameraNodeID(const char *cameraNodeID) {
    vtkSetAndObserveMRMLObjectMacro(this->CameraNode, nullptr);

    this->SetCameraNodeID(cameraNodeID);
    this->GetCameraNode();
}

vtkMRMLCameraNode *
vtkMRMLProjectiveTextureMappingDisplayNode::GetCameraNode() {
    if (this->CameraNodeID == nullptr) {
        vtkSetAndObserveMRMLObjectMacro(this->CameraNode, nullptr);
    } else if (this->GetScene() &&
               ((this->CameraNode != nullptr &&
                 strcmp(this->CameraNode->GetID(), this->CameraNodeID)) ||
                (this->CameraNode == nullptr))) {
        vtkMRMLNode *node = this->GetScene()->GetNodeByID(this->CameraNodeID);
        vtkSetAndObserveMRMLObjectMacro(this->CameraNode, vtkMRMLCameraNode::SafeDownCast(node));
    }
    return this->CameraNode;
}

vtkMRMLProjectiveTextureMappingDisplayNode::vtkMRMLProjectiveTextureMappingDisplayNode() {
    this->SurfaceModelNodeID = nullptr;
    this->SurfaceModelNode = nullptr;

    this->TextureNodeID = nullptr;
    this->TextureNode = nullptr;

    this->ColorMapNodeID = nullptr;
    this->ColorMapNode = nullptr;

    this->CameraNodeID = nullptr;
    this->CameraNode = nullptr;

    this->ZTolerance = 0.05;
}

vtkMRMLProjectiveTextureMappingDisplayNode::~vtkMRMLProjectiveTextureMappingDisplayNode() {
    if (this->SurfaceModelNodeID) {
        SetAndObserveSurfaceModelNodeID(nullptr);
    }

    if (this->TextureNodeID) {
        SetAndObserveTextureNodeID(nullptr);
    }

    if (this->ColorMapNodeID)  {
        SetAndObserveColorMapNodeID(nullptr);
    }

    if (this->CameraNodeID) {
        SetAndObserveCameraNodeID(nullptr);
    }
}

void
vtkMRMLProjectiveTextureMappingDisplayNode::ProcessMRMLEvents(vtkObject *caller,
                                                              unsigned long event,
                                                              void *callData) {
    this->Superclass::ProcessMRMLEvents(caller, event, callData);

    if (event == vtkCommand::ModifiedEvent) {
        this->InvokeEvent(vtkCommand::ModifiedEvent);
    }
    if (this->SurfaceModelNode != nullptr &&
        this->SurfaceModelNode == vtkMRMLModelNode::SafeDownCast(caller)) {
        if (event == vtkCommand::ModifiedEvent) {
            // surface model is like a 'member'
            this->InvokeEvent(vtkCommand::ModifiedEvent);
        } else {
            this->InvokeEvent(
                vtkMRMLProjectiveTextureMappingDisplayNode::SurfaceModelDataModifiedEvent);
        }
    } else if (this->TextureNode != nullptr &&
               this->TextureNode == vtkMRMLVolumeNode::SafeDownCast(caller)) {
        if (event == vtkCommand::ModifiedEvent) {
            // texture is like a 'member'
            this->InvokeEvent(vtkCommand::ModifiedEvent);
        } else {
            this->InvokeEvent(vtkMRMLProjectiveTextureMappingDisplayNode::TextureDataModifiedEvent);
        }
    } else if (this->ColorMapNode != nullptr &&
               this->ColorMapNode == vtkMRMLColorNode::SafeDownCast(caller)) {
        this->InvokeEvent(vtkMRMLProjectiveTextureMappingDisplayNode::ColorMapModifiedEvent);
    } else if (this->CameraNode != nullptr &&
               this->CameraNode == vtkMRMLCameraNode::SafeDownCast(caller)) {
        this->InvokeEvent(vtkMRMLProjectiveTextureMappingDisplayNode::CameraModifiedEvent);
    }
}
