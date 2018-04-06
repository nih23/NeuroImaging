#include "vtkMRMLImageFusionDisplayableManager.h"

#include <vtkAbstractVolumeMapper.h>
#include <vtkActor2D.h>
#include <vtkAlgorithmOutput.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkImageMapper.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkOpenGLVertexBufferObjectCache.h>
#include <vtkPolyDataNormals.h>
#include <vtkPropCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkWindowToImageFilter.h>

#include "vtkImageFusion.h"
#include "vtkOpenGLProjectiveTextureMapper.h"

#include <vtkMRMLCameraNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLVolumePropertyNode.h>
#include <vtkMRMLVolumeRenderingDisplayNode.h>

#include "vtkMRMLImageFusionDisplayNode.h"
#include "vtkMRMLProjectiveTextureMappingDisplayNode.h"

using namespace std;

vtkStandardNewMacro(vtkMRMLImageFusionDisplayableManager);

class vtkMRMLImageFusionDisplayableManager::vtkViewElement {
public:
    vtkViewElement();
    ~vtkViewElement();

    vtkNew<vtkVolume> Volume;
    vtkNew<vtkRenderer> VolumeRenderer;
    vtkNew<vtkRenderWindow> VolumeRenderingWindow;
    vtkNew<vtkWindowToImageFilter> VolumeRenderingImage;

    vtkNew<vtkActor> ProjectiveTextureMappingActor;
    vtkNew<vtkRenderer> ProjectiveTextureMappingRenderer;
    vtkNew<vtkRenderWindow> ProjectiveTextureMappingWindow;
    vtkNew<vtkWindowToImageFilter> ProjectiveTextureMappingImage;

    vtkNew<vtkImageFusion> FusedImage;
    vtkNew<vtkImageMapper> FusedImageMapper;
    vtkNew<vtkActor2D> FusedImageActor;

    vtkMRMLImageFusionDisplayNode *DisplayNode;
    vtkSmartPointer<vtkProp> MatchingVolumeRenderingProp;
    bool MatchingVolumeRenderingVisibility;
    vtkSmartPointer<vtkProp> MatchingProjectiveTextureMappingProp;
    bool MatchingProjectiveTextureMappingVisibility;
};

vtkMRMLImageFusionDisplayableManager::vtkViewElement::vtkViewElement() {
    VolumeRenderer->SetBackground(0.5, 0.5, 0.5);
    VolumeRenderingWindow->SetOffScreenRendering(true);
    VolumeRenderingImage->SetInputBufferTypeToRGBA();
    ProjectiveTextureMappingRenderer->SetBackground(0.5, 0.5, 0.5);
    ProjectiveTextureMappingImage->SetInputBufferTypeToRGBA();
    ProjectiveTextureMappingWindow->SetOffScreenRendering(true);

    VolumeRenderer->AddVolume(Volume);
    VolumeRenderingWindow->AddRenderer(VolumeRenderer);
    VolumeRenderingImage->SetInput(VolumeRenderingWindow);
    ProjectiveTextureMappingRenderer->AddActor(ProjectiveTextureMappingActor);
    ProjectiveTextureMappingWindow->AddRenderer(ProjectiveTextureMappingRenderer);
    ProjectiveTextureMappingImage->SetInput(ProjectiveTextureMappingWindow);

    FusedImageMapper->SetColorWindow(255);
    FusedImageMapper->SetColorLevel(127.5);
    FusedImageActor->SetVisibility(true);

    FusedImage->AddInputConnection(VolumeRenderingImage->GetOutputPort());
    FusedImage->AddInputConnection(ProjectiveTextureMappingImage->GetOutputPort());
    FusedImageMapper->SetInputConnection(FusedImage->GetOutputPort());
    FusedImageActor->SetMapper(FusedImageMapper);

    DisplayNode = nullptr;
    MatchingVolumeRenderingProp = nullptr;
    MatchingVolumeRenderingVisibility = true;
    MatchingProjectiveTextureMappingProp = nullptr;
    MatchingProjectiveTextureMappingVisibility = true;
}

vtkMRMLImageFusionDisplayableManager::vtkViewElement::~vtkViewElement() {}

vtkMRMLImageFusionDisplayableManager::vtkMRMLImageFusionDisplayableManager()
    : RendererProps{nullptr},
      RenderWindow{nullptr},
      RendererObserverManager{vtkObserverManager::New()},
      InRenderCallbackFlag{0} {
    RendererObserverManager->AssignOwner(this);
    RendererObserverManager->GetCallbackCommand()->SetClientData(reinterpret_cast<void *>(this));
    RendererObserverManager->GetCallbackCommand()->SetCallback(
        &vtkMRMLImageFusionDisplayableManager::RendererCallback);
}

vtkMRMLImageFusionDisplayableManager::~vtkMRMLImageFusionDisplayableManager() {
    this->ClearViewElements();
    this->RendererObserverManager->SetAndObserveObject(vtkObjectPointer(&(this->RendererProps)),
                                                       nullptr);
    this->RendererObserverManager->SetAndObserveObject(vtkObjectPointer(&(this->RenderWindow)),
                                                       nullptr);
    this->RendererObserverManager->Delete();
}

void
vtkMRMLImageFusionDisplayableManager::RendererCallback(vtkObject *caller,
                                                       unsigned long eid,
                                                       void *clientData,
                                                       void *callData) {
    auto self = reinterpret_cast<vtkMRMLImageFusionDisplayableManager *>(clientData);
    assert("Observed object is not a render window or a prop collection" &&
           (vtkRenderWindow::SafeDownCast(caller) || vtkPropCollection::SafeDownCast(caller)));

    vtkDebugWithObjectMacro(self, "In vtkImageFusionDisplayableManger RendererCallback");

    assert(self);

    self->SetInRendererCallbackFlag(self->GetInRendererCallbackFlag() + 1);
    self->ProcessRendererEvents(caller, eid, callData);
    self->SetInRendererCallbackFlag(self->GetInRendererCallbackFlag() - 1);
}

void
vtkMRMLImageFusionDisplayableManager::SetInRendererCallbackFlag(int newFlag) {
    this->InRenderCallbackFlag = newFlag;
}

int
vtkMRMLImageFusionDisplayableManager::GetInRendererCallbackFlag() {
    return this->InRenderCallbackFlag;
}

void
vtkMRMLImageFusionDisplayableManager::SetRenderer(vtkRenderer *newRenderer) {
    this->Superclass::SetRenderer(newRenderer);

    vtkPropCollection *newRendererProps = nullptr;
    vtkRenderWindow *newWindow = nullptr;
    if (newRenderer) {
        newRendererProps = newRenderer->GetViewProps();
        newWindow = newRenderer->GetRenderWindow();
        assert(newRendererProps);
        assert(newWindow);
    }

    this->RendererObserverManager->SetAndObserveObject(vtkObjectPointer(&(this->RendererProps)),
                                                       newRendererProps);

    vtkNew<vtkIntArray> renderEvents;
    renderEvents->InsertNextValue(vtkCommand::StartEvent);
    this->RendererObserverManager->SetAndObserveObjectEvents(
        vtkObjectPointer(&(this->RenderWindow)), newWindow, renderEvents);
}

void
vtkMRMLImageFusionDisplayableManager::UnobserveMRMLScene() {
    this->RemoveInvalidViewElements();
    this->RemoveDisplayNodeObservations();
    this->ClearViewElements();
}

void
vtkMRMLImageFusionDisplayableManager::OnMRMLSceneStartClose() {
    this->RemoveDisplayNodeObservations();
}

void
vtkMRMLImageFusionDisplayableManager::OnMRMLSceneEndClose() {
    this->RemoveInvalidViewElements();
    this->ClearViewElements();

    this->SetUpdateFromMRMLRequested(true);
    this->RequestRender();
}

void
vtkMRMLImageFusionDisplayableManager::UpdateFromMRMLScene() {
    this->RequestRender();
}

void
vtkMRMLImageFusionDisplayableManager::OnMRMLSceneNodeAdded(vtkMRMLNode *node) {
    if (!node->IsA("vtkMRMLImageFusionDisplayNode")) {
        return;
    }

    this->ActiveDisplayNodeID = node->GetID();
    this->SetUpdateFromMRMLRequested(1);

    // Escape if the scene a scene is being closed, imported or connected
    if (this->GetMRMLScene()->IsBatchProcessing()) {
        return;
    }

    this->RequestRender();
}

void
vtkMRMLImageFusionDisplayableManager::OnMRMLSceneNodeRemoved(vtkMRMLNode *node) {
    if (!node->IsA("vtkMRMLImageFusionDisplayNode")) {
        return;
    }

    if (this->ActiveDisplayNodeID == node->GetID()) {
        this->ActiveDisplayNodeID.clear();
    }
    this->SetUpdateFromMRMLRequested(1);

    // Escape if the scene a scene is being closed, imported or connected
    if (this->GetMRMLScene()->IsBatchProcessing()) {
        return;
    }

    this->RequestRender();
}

void
vtkMRMLImageFusionDisplayableManager::ProcessRendererEvents(vtkObject *caller,
                                                            unsigned long event,
                                                            void *callData) {
    if (vtkRenderWindow::SafeDownCast(caller) && event == vtkCommand::StartEvent) {
        this->UpdateInputImages();
    } else if (vtkPropCollection::SafeDownCast(caller) && event == vtkCommand::ModifiedEvent &&
               this->InRenderCallbackFlag == 1) {
        this->UpdateViewElementsFromMRML();
    }

    (void) callData;
}

void
vtkMRMLImageFusionDisplayableManager::ProcessMRMLNodesEvents(vtkObject *caller,
                                                             unsigned long event,
                                                             void *callData) {
    if (this->GetMRMLScene() == nullptr) {
        return;
    }
    if (this->GetInteractor() && this->GetInteractor()->GetRenderWindow() &&
        this->GetInteractor()->GetRenderWindow()->CheckInRenderStatus()) {
        vtkDebugMacro("skipping ProcessMRMLNodesEvents during render");
        return;
    }

    auto displayNode = vtkMRMLImageFusionDisplayNode::SafeDownCast(caller);
    if (displayNode && event == vtkCommand::ModifiedEvent) {
        this->SetUpdateFromMRMLRequested(true);
        bool isUpdating = this->GetMRMLScene()->IsBatchProcessing();
        if (!isUpdating) {
            this->RequestRender();
        }
    } else {
        this->Superclass::ProcessMRMLNodesEvents(caller, event, callData);
    }
}

void
vtkMRMLImageFusionDisplayableManager::UpdateFromMRML() {
    if (this->GetInteractor() && this->GetInteractor()->GetRenderWindow() &&
        this->GetInteractor()->GetRenderWindow()->CheckInRenderStatus()) {
        vtkDebugMacro("skipping update during render");
        return;
    }

    this->RemoveInvalidViewElements();

    this->AddNewViewElements();

    this->UpdateViewElementsFromMRML();

    this->SetUpdateFromMRMLRequested(false);
}

void
vtkMRMLImageFusionDisplayableManager::RemoveMRMLObservers() {
    this->RemoveDisplayNodeObservations();
    this->ClearViewElements();

    this->Superclass::RemoveMRMLObservers();
}

void
vtkMRMLImageFusionDisplayableManager::ClearViewElements() {
    for (pair<string, vtkViewElement *> const &idElementPair : this->ViewElements) {
        vtkViewElement *viewElement = idElementPair.second;

        this->GetRenderer()->RemoveActor(viewElement->FusedImageActor);

        if (viewElement->MatchingProjectiveTextureMappingProp) {
            bool visible = viewElement->MatchingProjectiveTextureMappingVisibility;
            viewElement->MatchingProjectiveTextureMappingProp->SetVisibility(visible);
        }

        if (viewElement->MatchingVolumeRenderingProp) {
            bool visible = viewElement->MatchingVolumeRenderingVisibility;
            viewElement->MatchingVolumeRenderingProp->SetVisibility(visible);
        }

        delete viewElement;
    }
    this->ViewElements.clear();
}

void
vtkMRMLImageFusionDisplayableManager::RemoveViewElementByID(const string &imageFusionDisplayID) {
    auto iter = this->ViewElements.find(imageFusionDisplayID);
    if (iter != this->ViewElements.end()) {
        vtkViewElement *viewElement = iter->second;
        this->GetRenderer()->RemoveActor(viewElement->FusedImageActor);

        if (viewElement->MatchingProjectiveTextureMappingProp) {
            bool visible = viewElement->MatchingProjectiveTextureMappingVisibility;
            viewElement->MatchingProjectiveTextureMappingProp->SetVisibility(visible);
        }

        if (viewElement->MatchingVolumeRenderingProp) {
            bool visible = viewElement->MatchingVolumeRenderingVisibility;
            viewElement->MatchingVolumeRenderingProp->SetVisibility(visible);
        }

        delete viewElement;
        this->ViewElements.erase(iter);
    }
}

void
vtkMRMLImageFusionDisplayableManager::RemoveInvalidViewElements() {
    vector<string> removedIDs;
    for (pair<string, vtkViewElement *> const &idElementPair : this->ViewElements) {
        string const &imageFusionDisplayNodeID = idElementPair.first;
        vtkViewElement *viewElement = idElementPair.second;
        vtkMRMLDisplayNode *imageFusionDisplayNode = vtkMRMLDisplayNode::SafeDownCast(
            this->GetMRMLScene() ? this->GetMRMLScene()->GetNodeByID(imageFusionDisplayNodeID)
                                 : nullptr);
        if (imageFusionDisplayNode == nullptr) {
            this->GetRenderer()->RemoveActor(viewElement->FusedImageActor);
            vtkSetMRMLNodeMacro(viewElement->DisplayNode, nullptr);
            removedIDs.push_back(imageFusionDisplayNodeID);
        }
    }

    for (string const &removedID : removedIDs) {
        this->RemoveViewElementByID(removedID);
    }
}

void
vtkMRMLImageFusionDisplayableManager::RemoveDisplayNodeObservations() {
    for (pair<string, vtkViewElement *> const &idElementPair : this->ViewElements) {
        vtkViewElement *viewElement = idElementPair.second;
        vtkSetMRMLNodeMacro(viewElement->DisplayNode, nullptr);
    }
}

void
vtkMRMLImageFusionDisplayableManager::AddNewViewElements() {
    vector<vtkMRMLNode *> displayNodes;
    vtkMRMLScene *scene = this->GetMRMLScene();
    if (scene) {
        scene->GetNodesByClass("vtkMRMLImageFusionDisplayNode", displayNodes);
    }

    for (vtkMRMLNode *displayNode : displayNodes) {
        auto imageFusionDisplayNode = vtkMRMLImageFusionDisplayNode::SafeDownCast(displayNode);
        assert(imageFusionDisplayNode);

        char *imageFusionDisplayNodeID = imageFusionDisplayNode->GetID();
        auto iter = this->ViewElements.find(imageFusionDisplayNodeID);
        // only observe new nodes
        if (iter == this->ViewElements.end()) {
            vtkViewElement *viewElement = new vtkViewElement;
            vtkSetAndObserveMRMLNodeMacro(viewElement->DisplayNode, imageFusionDisplayNode);
            this->ViewElements[imageFusionDisplayNodeID] = viewElement;

            this->GetRenderer()->AddActor(viewElement->FusedImageActor);
        }
    }
}

void
vtkMRMLImageFusionDisplayableManager::UpdateViewElementsFromMRML() {
    for (pair<string, vtkViewElement *> const &idElementPair : this->ViewElements) {
        std::string const &displayNodeID = idElementPair.first;
        vtkViewElement *viewElement = idElementPair.second;
        vtkActor2D *actor = viewElement->FusedImageActor;
        vtkMRMLImageFusionDisplayNode *displayNode = viewElement->DisplayNode;
        if (!displayNode) {
            continue;
        }

        bool actorRequired = this->IsDisplayNodeValid(displayNode) &&
            displayNode->GetVisibility() && displayNodeID == this->ActiveDisplayNodeID;

        vtkMRMLProjectiveTextureMappingDisplayNode *ptmDisplayNode =
            displayNode->GetProjectiveTextureMappingDisplayNode();
        vtkMRMLVolumeRenderingDisplayNode *dvrDisplayNode =
            displayNode->GetVolumeRenderingDisplayNode();

        vtkProp *volumeRenderingProp = nullptr;
        vtkProp *projectiveTextureMappingProp = nullptr;
        if (actorRequired) {
            volumeRenderingProp = this->FindMatchingProp(dvrDisplayNode);
            projectiveTextureMappingProp = this->FindMatchingProp(ptmDisplayNode);
            actorRequired = volumeRenderingProp && projectiveTextureMappingProp;
        }

        // remove the actor if the display node was valid before
        if (!actorRequired) {
            actor->SetVisibility(false);

            if (viewElement->MatchingProjectiveTextureMappingProp) {
                bool visible = viewElement->MatchingProjectiveTextureMappingVisibility;
                viewElement->MatchingProjectiveTextureMappingProp->SetVisibility(visible);
            }

            if (viewElement->MatchingVolumeRenderingProp) {
                bool visible = viewElement->MatchingVolumeRenderingVisibility;
                viewElement->MatchingVolumeRenderingProp->SetVisibility(visible);
            }

            continue;
        }

        if (viewElement->MatchingVolumeRenderingProp != volumeRenderingProp) {
            viewElement->MatchingVolumeRenderingProp = volumeRenderingProp;
            viewElement->MatchingVolumeRenderingVisibility = volumeRenderingProp->GetVisibility();
        }

        if (viewElement->MatchingProjectiveTextureMappingProp != projectiveTextureMappingProp) {
            viewElement->MatchingProjectiveTextureMappingProp = projectiveTextureMappingProp;
            viewElement->MatchingProjectiveTextureMappingVisibility =
                projectiveTextureMappingProp->GetVisibility();
        }

        viewElement->Volume->ShallowCopy(volumeRenderingProp);
        viewElement->Volume->VisibilityOn();

        vtkActor *ptmActor = vtkActor::SafeDownCast(projectiveTextureMappingProp);
        viewElement->ProjectiveTextureMappingActor->ShallowCopy(projectiveTextureMappingProp);
        viewElement->ProjectiveTextureMappingActor->SetForceTranslucent(
            ptmActor->GetForceTranslucent());
        viewElement->ProjectiveTextureMappingActor->SetForceOpaque(ptmActor->GetForceOpaque());
        viewElement->ProjectiveTextureMappingActor->VisibilityOn();

        volumeRenderingProp->VisibilityOff();
        projectiveTextureMappingProp->VisibilityOff();

        viewElement->FusedImage->SetMethod(displayNode->GetImageFusionMethod());
        viewElement->FusedImage->SetFirstInputWeight(displayNode->GetVolumeRenderingWeight());

        actor->SetVisibility(true);
    }
}

bool
vtkMRMLImageFusionDisplayableManager::IsDisplayNodeValid(
    vtkMRMLImageFusionDisplayNode *node) const {
    if (node == nullptr) {
        return false;
    }

    vtkMRMLVolumeRenderingDisplayNode *dvrDisplayNode = node->GetVolumeRenderingDisplayNode();
    vtkMRMLProjectiveTextureMappingDisplayNode *ptmDisplayNode =
        node->GetProjectiveTextureMappingDisplayNode();

    return this->IsDisplayNodeValid(dvrDisplayNode) && this->IsDisplayNodeValid(ptmDisplayNode);
}

bool
vtkMRMLImageFusionDisplayableManager::IsDisplayNodeValid(
    vtkMRMLVolumeRenderingDisplayNode *node) const {
    if (node == nullptr) {
        return false;
    }

    vtkMRMLVolumeNode *volumeNode = node->GetVolumeNode();
    vtkMRMLVolumePropertyNode *volumePropertyNode = node->GetVolumePropertyNode();

    if (!volumeNode || !volumePropertyNode) {
        return false;
    }

    vtkAlgorithmOutput *volumeImageDataConnection = volumeNode->GetImageDataConnection();
    vtkVolumeProperty *volumeProperty = volumePropertyNode->GetVolumeProperty();

    return volumeImageDataConnection && volumeProperty;
}

bool
vtkMRMLImageFusionDisplayableManager::IsDisplayNodeValid(
    vtkMRMLProjectiveTextureMappingDisplayNode *node) const {
    if (node == nullptr) {
        return false;
    }
    vtkMRMLModelNode *surfaceModelNode = node->GetSurfaceModelNode();
    vtkMRMLVolumeNode *textureNode = node->GetTextureNode();
    vtkMRMLCameraNode *cameraNode = node->GetCameraNode();

    if (!surfaceModelNode || !textureNode || !cameraNode) {
        return false;
    }

    vtkAlgorithmOutput *surfaceModelMeshConnection = surfaceModelNode->GetMeshConnection();
    vtkAlgorithmOutput *textureImageDataConnection = textureNode->GetImageDataConnection();

    return surfaceModelMeshConnection && textureImageDataConnection;
}

vtkProp *
vtkMRMLImageFusionDisplayableManager::FindMatchingProp(vtkMRMLVolumeRenderingDisplayNode *node) {
    vtkCollectionSimpleIterator it;
    assert(this->RendererProps);
    this->RendererProps->InitTraversal(it);
    vtkProp *currentProp = nullptr;
    vtkProp *matchingProp = nullptr;

    vtkMRMLVolumePropertyNode *volumePropertyNode = node->GetVolumePropertyNode();
    assert(volumePropertyNode);
    vtkVolumeProperty *volumeProperty = volumePropertyNode->GetVolumeProperty();
    assert(volumeProperty);

    vtkMRMLVolumeNode *volumeNode = node->GetVolumeNode();
    assert(volumeNode);
    vtkAlgorithmOutput *volumeImageDataConnection = volumeNode->GetImageDataConnection();
    assert(volumeImageDataConnection);

    while ((currentProp = this->RendererProps->GetNextProp(it))) {
        auto currentVolume = vtkVolume::SafeDownCast(currentProp);
        if (!currentVolume || currentVolume->GetProperty() != volumeProperty) {
            continue;
        }

        vtkAbstractVolumeMapper *mapper = currentVolume->GetMapper();
        if (!mapper) {
            continue;
        }

        if (mapper->GetNumberOfInputConnections(0) < 1) {
            continue;
        }

        vtkAlgorithmOutput *dataConnection = mapper->GetInputConnection(0, 0);
        if (!dataConnection || dataConnection != volumeImageDataConnection) {
            continue;
        }

        if (matchingProp != nullptr) {
            vtkWarningMacro(
                << "Warning: multiple props that match the given volume rendering display node");
        }

        matchingProp = currentProp;
    }
    return matchingProp;
}

vtkProp *
vtkMRMLImageFusionDisplayableManager::FindMatchingProp(
    vtkMRMLProjectiveTextureMappingDisplayNode *node) {
    vtkCollectionSimpleIterator it;
    assert(this->RendererProps);
    this->RendererProps->InitTraversal(it);
    vtkProp *currentProp = nullptr;
    vtkProp *matchingProp = nullptr;

    vtkMRMLModelNode *surfaceModelNode = node->GetSurfaceModelNode();
    assert(surfaceModelNode);
    vtkAlgorithmOutput *surfaceModelMeshConnection = surfaceModelNode->GetMeshConnection();
    assert(surfaceModelMeshConnection);

    vtkMRMLVolumeNode *textureNode = node->GetTextureNode();
    assert(textureNode);
    vtkAlgorithmOutput *textureImageDataConnection = textureNode->GetImageDataConnection();
    assert(textureImageDataConnection);

    while ((currentProp = this->RendererProps->GetNextProp(it))) {
        auto currentActor = vtkActor::SafeDownCast(currentProp);
        if (!currentActor) {
            continue;
        }

        auto mapper = vtkOpenGLProjectiveTextureMapper::SafeDownCast(currentActor->GetMapper());
        if (!mapper) {
            continue;
        }

        // skip surface normals algorithm output

        if (mapper->GetNumberOfInputConnections(0) < 1) {
            continue;
        }

        vtkAlgorithmOutput *meshConnection = mapper->GetInputConnection(0, 0);
        if (meshConnection) {
            auto producer = vtkPolyDataNormals::SafeDownCast(meshConnection->GetProducer());
            if (producer && producer->GetNumberOfInputConnections(0) >= 1) {
                meshConnection = producer->GetInputConnection(0, 0);
            }
        }

        if (!meshConnection || meshConnection != surfaceModelMeshConnection) {
            cout << "Mesh mismatch" << endl;
            continue;
        }

        vtkAlgorithmOutput *textureConnection = mapper->GetProjectedTextureConnection();
        if (!textureConnection || textureConnection != textureImageDataConnection) {
            cout << "texture mismatch" << endl;
            continue;
        }

        if (matchingProp != nullptr) {
            vtkWarningMacro(
                << "Warning: multiple props that match the given volume rendering display node");
        }

        matchingProp = currentProp;
    }
    return matchingProp;
}

void
vtkMRMLImageFusionDisplayableManager::UpdateInputImages() {
    auto iter = this->ViewElements.find(this->ActiveDisplayNodeID);
    if (iter == this->ViewElements.end()) {
        return;
    }

    vtkViewElement *viewElement = iter->second;

    if (viewElement->VolumeRenderer->GetViewProps()->GetNumberOfItems() < 1 ||
        viewElement->ProjectiveTextureMappingRenderer->GetViewProps()->GetNumberOfItems() < 1) {
        return;
    }

    vtkRenderer *renderer = this->GetRenderer();
    vtkRenderWindow *window = this->GetInteractor()->GetRenderWindow();
    viewElement->VolumeRenderer->GetActiveCamera()->DeepCopy(renderer->GetActiveCamera());
    viewElement->VolumeRenderingWindow->SetSize(window->GetSize());
    viewElement->VolumeRenderingImage->Modified();

    viewElement->ProjectiveTextureMappingRenderer->GetActiveCamera()->DeepCopy(
        renderer->GetActiveCamera());
    viewElement->ProjectiveTextureMappingWindow->SetSize(window->GetSize());
    viewElement->ProjectiveTextureMappingImage->Modified();
}
