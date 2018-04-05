#include "vtkMRMLImageFusionDisplayNode.h"

#include <vtkMRMLScene.h>
#include <vtkMRMLVolumeRenderingDisplayNode.h>

#include "vtkMRMLProjectiveTextureMappingDisplayNode.h"

#include <vtkCommand.h>

vtkMRMLNodeNewMacro(vtkMRMLImageFusionDisplayNode);

vtkCxxSetReferenceStringMacro(vtkMRMLImageFusionDisplayNode, VolumeRenderingDisplayNodeID);
vtkCxxSetReferenceStringMacro(vtkMRMLImageFusionDisplayNode, ProjectiveTextureMappingDisplayNodeID);

void
vtkMRMLImageFusionDisplayNode::PrintSelf(std::ostream &os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);

    os << "VolumeRenderingDisplayNodeID: "
       << (this->VolumeRenderingDisplayNodeID ? this->VolumeRenderingDisplayNodeID : "None")
       << "\n";
    os << "ProjectiveTextureMappingDisplayNodeID: "
       << (this->ProjectiveTextureMappingDisplayNodeID ? this->ProjectiveTextureMappingDisplayNodeID
                                                       : "None")
       << "\n";
}

void
vtkMRMLImageFusionDisplayNode::ReadXMLAttributes(const char **atts) {
    this->Superclass::ReadXMLAttributes(atts);

    const char *attName;
    const char *attValue;
    while (*atts != nullptr) {
        attName = *(atts++);
        attValue = *(atts++);
        if (!strcmp(attName, "volumeRenderingDisplayNodeID")) {
            this->SetVolumeRenderingDisplayNodeID(attValue);
        } else if (!strcmp(attName, "projectiveTextureMappingDisplayNodeID")) {
            this->SetProjectiveTextureMappingDisplayNodeID(attValue);
        }
    }
}

void
vtkMRMLImageFusionDisplayNode::WriteXML(std::ostream &of, int indent) {
    this->Superclass::WriteXML(of, indent);
    of << " volumeRenderingDisplayNodeID=\""
       << (this->VolumeRenderingDisplayNodeID ? this->VolumeRenderingDisplayNodeID : "NULL")
       << "\"";
    of << " projectiveTextureMappingDisplayNodeID=\""
       << (this->ProjectiveTextureMappingDisplayNodeID ? this->ProjectiveTextureMappingDisplayNodeID
                                                       : "NULL")
       << "\"";
}

void
vtkMRMLImageFusionDisplayNode::Copy(vtkMRMLNode *anode) {
    int wasModifying = this->StartModify();
    this->Superclass::Copy(anode);
    auto node = vtkMRMLImageFusionDisplayNode::SafeDownCast(anode);

    this->SetVolumeRenderingDisplayNodeID(node->GetVolumeRenderingDisplayNodeID());
    this->SetProjectiveTextureMappingDisplayNodeID(
        node->GetProjectiveTextureMappingDisplayNodeID());

    this->EndModify(wasModifying);
}

void
vtkMRMLImageFusionDisplayNode::SetSceneReferences() {
    this->Superclass::SetSceneReferences();

    this->Scene->AddReferencedNodeID(this->VolumeRenderingDisplayNodeID, this);
    this->Scene->AddReferencedNodeID(this->ProjectiveTextureMappingDisplayNodeID, this);
}

void
vtkMRMLImageFusionDisplayNode::UpdateReferenceID(const char *oldID, const char *newID) {
    this->Superclass::UpdateReferenceID(oldID, newID);

    if (this->VolumeRenderingDisplayNodeID && !strcmp(oldID, this->VolumeRenderingDisplayNodeID)) {
        this->SetAndObserveVolumeRenderingDisplayNodeID(newID);
    } else if (this->ProjectiveTextureMappingDisplayNodeID &&
               !strcmp(oldID, this->ProjectiveTextureMappingDisplayNodeID)) {
        this->SetAndObserveProjectiveTextureMappingDisplayNodeID(newID);
    }
}

void
vtkMRMLImageFusionDisplayNode::UpdateReferences() {
    this->Superclass::UpdateReferences();

    if (this->VolumeRenderingDisplayNodeID != nullptr &&
        this->Scene->GetNodeByID(this->VolumeRenderingDisplayNodeID) == nullptr) {
        this->SetAndObserveVolumeRenderingDisplayNodeID(nullptr);
    }
    if (this->ProjectiveTextureMappingDisplayNodeID != nullptr &&
        this->Scene->GetNodeByID(this->ProjectiveTextureMappingDisplayNodeID) == nullptr) {
        this->SetAndObserveProjectiveTextureMappingDisplayNodeID(nullptr);
    }
}

void
vtkMRMLImageFusionDisplayNode::UpdateScene(vtkMRMLScene *scene) {
    this->Superclass::UpdateScene(scene);

    this->SetAndObserveVolumeRenderingDisplayNodeID(this->VolumeRenderingDisplayNodeID);
    this->SetAndObserveProjectiveTextureMappingDisplayNodeID(
        this->ProjectiveTextureMappingDisplayNodeID);
}

const char *
vtkMRMLImageFusionDisplayNode::GetNodeTagName() {
    return "ImageFusion";
}

void
vtkMRMLImageFusionDisplayNode::SetAndObserveVolumeRenderingDisplayNodeID(
    const char *volumeRenderingDisplayNodeID) {
    vtkSetAndObserveMRMLObjectMacro(this->VolumeRenderingDisplayNode, nullptr);

    this->SetVolumeRenderingDisplayNodeID(volumeRenderingDisplayNodeID);
    vtkMRMLVolumeRenderingDisplayNode *node = this->GetVolumeRenderingDisplayNode();
    (void) node; // only used to update & observe the node variable
}

vtkMRMLVolumeRenderingDisplayNode *
vtkMRMLImageFusionDisplayNode::GetVolumeRenderingDisplayNode() {
    if (this->VolumeRenderingDisplayNodeID == nullptr) {
        vtkSetAndObserveMRMLObjectMacro(this->VolumeRenderingDisplayNode, nullptr);
    }
    if (this->GetScene() &&
        ((this->VolumeRenderingDisplayNode != nullptr && this->VolumeRenderingDisplayNodeID &&
          strcmp(this->VolumeRenderingDisplayNode->GetID(), this->VolumeRenderingDisplayNodeID)) ||
         this->VolumeRenderingDisplayNode == nullptr)) {
        vtkMRMLNode *node = this->GetScene()->GetNodeByID(this->VolumeRenderingDisplayNodeID);
        vtkSetAndObserveMRMLObjectMacro(this->VolumeRenderingDisplayNode,
                                        vtkMRMLVolumeRenderingDisplayNode::SafeDownCast(node));
    }
    return this->VolumeRenderingDisplayNode;
}

void
vtkMRMLImageFusionDisplayNode::SetAndObserveProjectiveTextureMappingDisplayNodeID(
    const char *projectiveTextureMappingDisplayNodeID) {
    vtkSetAndObserveMRMLObjectMacro(this->ProjectiveTextureMappingDisplayNode, nullptr);

    this->SetProjectiveTextureMappingDisplayNodeID(projectiveTextureMappingDisplayNodeID);
    vtkMRMLProjectiveTextureMappingDisplayNode *node =
        this->GetProjectiveTextureMappingDisplayNode();
    (void) node; // only used to update the node variable
}

vtkMRMLProjectiveTextureMappingDisplayNode *
vtkMRMLImageFusionDisplayNode::GetProjectiveTextureMappingDisplayNode() {
    if (this->ProjectiveTextureMappingDisplayNodeID == nullptr) {
        vtkSetAndObserveMRMLObjectMacro(this->ProjectiveTextureMappingDisplayNode, nullptr);
    } else if (this->GetScene() &&
               ((this->ProjectiveTextureMappingDisplayNode != nullptr &&
                 strcmp(this->ProjectiveTextureMappingDisplayNode->GetID(),
                        this->ProjectiveTextureMappingDisplayNodeID)) ||
                this->ProjectiveTextureMappingDisplayNode == nullptr)) {
        vtkMRMLNode *node =
            this->GetScene()->GetNodeByID(this->ProjectiveTextureMappingDisplayNodeID);
        vtkSetAndObserveMRMLObjectMacro(
            this->ProjectiveTextureMappingDisplayNode,
            vtkMRMLProjectiveTextureMappingDisplayNode::SafeDownCast(node));
    }
    return this->ProjectiveTextureMappingDisplayNode;
}

vtkMRMLImageFusionDisplayNode::vtkMRMLImageFusionDisplayNode() {
    this->VolumeRenderingDisplayNodeID = nullptr;
    this->VolumeRenderingDisplayNode = nullptr;

    this->ProjectiveTextureMappingDisplayNodeID = nullptr;
    this->ProjectiveTextureMappingDisplayNode = nullptr;

    this->ImageFusionMethod = GUIDED_FILTERING;
    this->VolumeRenderingWeight = 0.5;
}

vtkMRMLImageFusionDisplayNode::~vtkMRMLImageFusionDisplayNode() {
    if (this->VolumeRenderingDisplayNodeID) {
        SetAndObserveVolumeRenderingDisplayNodeID(nullptr);
    }

    if (this->ProjectiveTextureMappingDisplayNodeID) {
        SetAndObserveProjectiveTextureMappingDisplayNodeID(nullptr);
    }
}

void
vtkMRMLImageFusionDisplayNode::ProcessMRMLEvents(vtkObject *caller,
                                                 unsigned long event,
                                                 void *callData) {
    this->Superclass::ProcessMRMLEvents(caller, event, callData);

    if (event == vtkCommand::ModifiedEvent) {
        this->InvokeEvent(vtkCommand::ModifiedEvent);
    }
    if (this->VolumeRenderingDisplayNode != nullptr &&
        this->VolumeRenderingDisplayNode ==
            vtkMRMLVolumeRenderingDisplayNode::SafeDownCast(caller)) {
        if (event == vtkCommand::ModifiedEvent) {
            // volume rendering display is like a 'member'
            this->InvokeEvent(vtkCommand::ModifiedEvent);
        }
    } else if (this->ProjectiveTextureMappingDisplayNode != nullptr &&
               this->ProjectiveTextureMappingDisplayNode ==
                   vtkMRMLProjectiveTextureMappingDisplayNode::SafeDownCast(caller)) {
        if (event == vtkCommand::ModifiedEvent) {
            // projective texture mapping display is like a 'member'
            this->InvokeEvent(vtkCommand::ModifiedEvent);
        }
    }
}
