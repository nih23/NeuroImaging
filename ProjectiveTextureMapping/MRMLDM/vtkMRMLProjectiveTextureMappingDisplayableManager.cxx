#include "vtkMRMLProjectiveTextureMappingDisplayableManager.h"

#include <vtkMRMLCameraNode.h>
#include <vtkMRMLColorNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLVolumeNode.h>

#include "vtkMRMLProjectiveTextureMappingDisplayNode.h"

#include <vtkAbstractVolumeMapper.h>
#include <vtkAlgorithmOutput.h>
#include <vtkCamera.h>
#include <vtkImageShiftScale.h>
#include <vtkObjectFactory.h>
#include <vtkPNGWriter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkWindowToImageFilter.h>

#include "vtkOpenGLProjectiveTextureMapper.h"

using namespace std;

vtkStandardNewMacro(vtkMRMLProjectiveTextureMappingDisplayableManager);

class vtkMRMLProjectiveTextureMappingDisplayableManager::vtkViewElement {
public:
    vtkViewElement();
    ~vtkViewElement();

    vtkNew<vtkPolyDataNormals> SurfaceModelGeometry;

    vtkNew<vtkPolyDataMapper> DepthSurfaceModelMapper;
    vtkNew<vtkActor> DepthSurfaceModelActor;
    vtkNew<vtkRenderer> DepthRenderer;
    vtkNew<vtkRenderWindow> DepthRenderWindow;
    vtkNew<vtkWindowToImageFilter> DepthBuffer;
    vtkNew<vtkImageShiftScale> DepthImage;

    vtkNew<vtkOpenGLProjectiveTextureMapper> Mapper;
    vtkNew<vtkActor> Actor;

    vtkMRMLProjectiveTextureMappingDisplayNode *DisplayNode;
};

vtkMRMLProjectiveTextureMappingDisplayableManager::vtkViewElement::vtkViewElement() {
    SurfaceModelGeometry->SetFeatureAngle(60.0);
    DepthRenderWindow->SetOffScreenRendering(true);
    DepthBuffer->SetInputBufferTypeToZBuffer();
    DepthImage->SetOutputScalarTypeToUnsignedChar();
    DepthImage->SetScale(255);

    DepthSurfaceModelMapper->SetInputConnection(SurfaceModelGeometry->GetOutputPort());
    DepthSurfaceModelActor->SetMapper(DepthSurfaceModelMapper);
    DepthRenderer->AddActor(DepthSurfaceModelActor);
    DepthRenderWindow->AddRenderer(DepthRenderer);
    DepthBuffer->SetInput(DepthRenderWindow);
    DepthImage->SetInputConnection(DepthBuffer->GetOutputPort());

    Actor->GetProperty()->SetLighting(false);
    Actor->GetProperty()->SetBackfaceCulling(true);
    Actor->SetVisibility(false);

    Mapper->SetInputConnection(SurfaceModelGeometry->GetOutputPort());
    Mapper->SetDepthRenderer(DepthRenderer);
    Mapper->SetDepthConnection(DepthImage->GetOutputPort());
    Actor->SetMapper(Mapper);

    DisplayNode = nullptr;
}

vtkMRMLProjectiveTextureMappingDisplayableManager::vtkViewElement::~vtkViewElement() {}

vtkMRMLProjectiveTextureMappingDisplayableManager::
    vtkMRMLProjectiveTextureMappingDisplayableManager() {}

vtkMRMLProjectiveTextureMappingDisplayableManager::
    ~vtkMRMLProjectiveTextureMappingDisplayableManager() {
    this->ClearViewElements();
}

void
vtkMRMLProjectiveTextureMappingDisplayableManager::UnobserveMRMLScene() {
    this->RemoveInvalidViewElements();
    this->RemoveDisplayNodeObservations();
    this->ClearViewElements();
}

void
vtkMRMLProjectiveTextureMappingDisplayableManager::OnMRMLSceneStartClose() {
    this->RemoveDisplayNodeObservations();
}

void
vtkMRMLProjectiveTextureMappingDisplayableManager::OnMRMLSceneEndClose() {
    this->RemoveInvalidViewElements();
    this->ClearViewElements();

    this->SetUpdateFromMRMLRequested(true);
    this->RequestRender();
}

void
vtkMRMLProjectiveTextureMappingDisplayableManager::UpdateFromMRMLScene() {
    this->RequestRender();
}

void
vtkMRMLProjectiveTextureMappingDisplayableManager::OnMRMLSceneNodeAdded(vtkMRMLNode *node) {
    if (!node->IsA("vtkMRMLProjectiveTextureMappingDisplayNode")) {
        return;
    }

    this->SetUpdateFromMRMLRequested(1);

    // Escape if the scene a scene is being closed, imported or connected
    if (this->GetMRMLScene()->IsBatchProcessing()) {
        return;
    }

    this->RequestRender();
}

void
vtkMRMLProjectiveTextureMappingDisplayableManager::OnMRMLSceneNodeRemoved(vtkMRMLNode *node) {
    if (!node->IsA("vtkMRMLProjectiveTextureMappingDisplayNode")) {
        return;
    }

    this->SetUpdateFromMRMLRequested(1);

    // Escape if the scene a scene is being closed, imported or connected
    if (this->GetMRMLScene()->IsBatchProcessing()) {
        return;
    }

    this->RequestRender();
}

void
vtkMRMLProjectiveTextureMappingDisplayableManager::ProcessMRMLNodesEvents(vtkObject *caller,
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

    bool isUpdating = this->GetMRMLScene()->IsBatchProcessing();
    auto displayNode = vtkMRMLProjectiveTextureMappingDisplayNode::SafeDownCast(caller);
    if (displayNode) {
        // There is no need to request a render (which can be expensive if the
        // volume rendering is on) if nothing visible has changed.
        bool requestRender = true;
        switch (event) {
        case vtkMRMLProjectiveTextureMappingDisplayNode::SurfaceModelDataModifiedEvent:
        case vtkMRMLProjectiveTextureMappingDisplayNode::TextureDataModifiedEvent:
        case vtkMRMLProjectiveTextureMappingDisplayNode::ColorMapModifiedEvent:
        case vtkMRMLProjectiveTextureMappingDisplayNode::CameraModifiedEvent:
            requestRender = this->IsRerenderingRequired(displayNode);
            if (requestRender) {
                this->UpdateDepthBuffer(displayNode);
            }
            break;
        case vtkCommand::ModifiedEvent:
            this->SetUpdateFromMRMLRequested(true);
            break;
        }

        if (!isUpdating && requestRender) {
            this->RequestRender();
        }
    } else {
        this->Superclass::ProcessMRMLNodesEvents(caller, event, callData);
    }
}

void
vtkMRMLProjectiveTextureMappingDisplayableManager::UpdateFromMRML() {
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
vtkMRMLProjectiveTextureMappingDisplayableManager::RemoveMRMLObservers() {
    this->RemoveDisplayNodeObservations();
    this->ClearViewElements();

    this->Superclass::RemoveMRMLObservers();
}

void
vtkMRMLProjectiveTextureMappingDisplayableManager::ClearViewElements() {
    for (pair<string, vtkViewElement *> const &idElementPair : this->ViewElements) {
        vtkViewElement *viewElement = idElementPair.second;
        this->GetRenderer()->RemoveActor(viewElement->Actor);
        delete viewElement;
    }
    this->ViewElements.clear();
}

void
vtkMRMLProjectiveTextureMappingDisplayableManager::RemoveViewElementByID(
    const string &ptmDisplayID) {
    auto iter = this->ViewElements.find(ptmDisplayID);
    if (iter != this->ViewElements.end()) {
        vtkViewElement *viewElement = iter->second;
        this->GetRenderer()->RemoveActor(viewElement->Actor);
        delete viewElement;
        this->ViewElements.erase(iter);
    }
}

void
vtkMRMLProjectiveTextureMappingDisplayableManager::RemoveInvalidViewElements() {
    vector<string> removedIDs;
    for (pair<string, vtkViewElement *> const &idElementPair : this->ViewElements) {
        string const &ptmDisplayNodeID = idElementPair.first;
        vtkViewElement *viewElement = idElementPair.second;
        vtkMRMLDisplayNode *ptmDisplayNode = vtkMRMLDisplayNode::SafeDownCast(
            this->GetMRMLScene() ? this->GetMRMLScene()->GetNodeByID(ptmDisplayNodeID) : nullptr);
        if (ptmDisplayNode == nullptr) {
            this->GetRenderer()->RemoveActor(viewElement->Actor);
            vtkSetMRMLNodeMacro(viewElement->DisplayNode, nullptr);
            removedIDs.push_back(ptmDisplayNodeID);
        }
    }

    for (string const &removedID : removedIDs) {
        this->RemoveViewElementByID(removedID);
    }
}

void
vtkMRMLProjectiveTextureMappingDisplayableManager::RemoveDisplayNodeObservations() {
    for (pair<string, vtkViewElement *> const &idElementPair : this->ViewElements) {
        vtkViewElement *viewElement = idElementPair.second;
        vtkSetMRMLNodeMacro(viewElement->DisplayNode, nullptr);
    }
}

void
vtkMRMLProjectiveTextureMappingDisplayableManager::AddNewViewElements() {
    vector<vtkMRMLNode *> displayNodes;
    vtkMRMLScene *scene = this->GetMRMLScene();
    if (scene) {
        scene->GetNodesByClass("vtkMRMLProjectiveTextureMappingDisplayNode", displayNodes);
    }

    for (vtkMRMLNode *displayNode : displayNodes) {
        auto ptmDisplayNode = vtkMRMLProjectiveTextureMappingDisplayNode::SafeDownCast(displayNode);
        assert(ptmDisplayNode);

        char *ptmDisplayNodeID = ptmDisplayNode->GetID();
        auto iter = this->ViewElements.find(ptmDisplayNodeID);
        // only observe new nodes
        if (iter == this->ViewElements.end()) {
            vtkViewElement *viewElement = new vtkViewElement;
            vtkNew<vtkIntArray> events;
            events->InsertNextValue(vtkCommand::ModifiedEvent);
            events->InsertNextValue(
                vtkMRMLProjectiveTextureMappingDisplayNode::CameraModifiedEvent);
            events->InsertNextValue(
                vtkMRMLProjectiveTextureMappingDisplayNode::TextureDataModifiedEvent);
            events->InsertNextValue(
                vtkMRMLProjectiveTextureMappingDisplayNode::ColorMapModifiedEvent);
            events->InsertNextValue(
                vtkMRMLProjectiveTextureMappingDisplayNode::SurfaceModelDataModifiedEvent);

            vtkSetAndObserveMRMLNodeEventsMacro(viewElement->DisplayNode, ptmDisplayNode, events);
            this->ViewElements[ptmDisplayNodeID] = viewElement;

            this->GetRenderer()->AddActor(viewElement->Actor);
        }
    }
}

void
vtkMRMLProjectiveTextureMappingDisplayableManager::UpdateViewElementsFromMRML() {
    for (pair<string, vtkViewElement *> const &idElementPair : this->ViewElements) {
        vtkViewElement *viewElement = idElementPair.second;
        vtkActor *actor = viewElement->Actor;
        vtkMRMLProjectiveTextureMappingDisplayNode *displayNode = viewElement->DisplayNode;
        assert(displayNode);

        // hide the actor if the display node was valid before
        if (!this->IsDisplayNodeValid(displayNode) || !displayNode->GetVisibility()) {
            actor->SetVisibility(false);
            continue;
        }

        vtkMRMLModelNode *surfaceModelNode = displayNode->GetSurfaceModelNode();
        vtkMRMLVolumeNode *textureNode = displayNode->GetTextureNode();
        vtkMRMLColorNode *colorMapNode = displayNode->GetColorMapNode();

        vtkAlgorithmOutput *surfaceModelMeshConnection = surfaceModelNode->GetMeshConnection();
        vtkAlgorithmOutput *textureImageDataConnection = textureNode->GetImageDataConnection();

        viewElement->SurfaceModelGeometry->SetInputConnection(surfaceModelMeshConnection);
        if (colorMapNode) {
            vtkScalarsToColors *colorMap = colorMapNode->GetScalarsToColors();
            assert(colorMap);
            viewElement->Mapper->SetProjectedTextureConnection(textureImageDataConnection,
                                                               colorMap);
        } else {
            viewElement->Mapper->SetProjectedTextureConnection(textureImageDataConnection);
        }

        this->UpdateDepthBuffer(displayNode);

        vtkAlgorithm *textureImageAlgorithm = textureImageDataConnection->GetProducer();
        textureImageAlgorithm->Update();
        vtkImageData *textureImageData =
            vtkImageData::SafeDownCast(textureImageAlgorithm->GetOutputDataObject(0));
        int dims[3];
        textureImageData->GetDimensions(dims);
        assert(dims[2] == 1);
        double aspectRatio = dims[0] * 1.0 / dims[1];

        viewElement->SurfaceModelGeometry->Update();
        vtkPolyData *surfaceModelPolyData = viewElement->SurfaceModelGeometry->GetOutput();
        double meshLength = surfaceModelPolyData->GetLength();
        double relativeZTolerance = displayNode->GetZTolerance();
        double zTolerance = meshLength * relativeZTolerance;

        viewElement->Mapper->SetAspectRatio(aspectRatio);
        viewElement->Mapper->SetZTolerance(zTolerance);

        actor->SetVisibility(true);
    }
}

void
vtkMRMLProjectiveTextureMappingDisplayableManager::UpdateDepthBuffer(
    vtkMRMLProjectiveTextureMappingDisplayNode *displayNode) {
    assert(displayNode);

    auto idElementIter = this->ViewElements.find(displayNode->GetID());
    assert(idElementIter != this->ViewElements.end());

    vtkMRMLVolumeNode *textureNode = displayNode->GetTextureNode();
    vtkMRMLCameraNode *cameraNode = displayNode->GetCameraNode();

    vtkAlgorithmOutput *textureImageDataConnection = textureNode->GetImageDataConnection();
    vtkAlgorithm *textureImageAlgorithm = textureImageDataConnection->GetProducer();
    textureImageAlgorithm->Update();
    vtkImageData *textureImageData =
        vtkImageData::SafeDownCast(textureImageAlgorithm->GetOutputDataObject(0));

    vtkCamera *camera = cameraNode->GetCamera();

    int dims[3];
    textureImageData->GetDimensions(dims);

    vtkViewElement *viewElement = idElementIter->second;
    viewElement->DepthRenderer->GetActiveCamera()->DeepCopy(camera);
    viewElement->DepthRenderer->ResetCameraClippingRange();
    viewElement->DepthRenderWindow->SetSize(dims[0], dims[1]);
    viewElement->DepthRenderWindow->Render();
    viewElement->DepthBuffer->Modified();
    viewElement->DepthBuffer->Update();
}

bool
vtkMRMLProjectiveTextureMappingDisplayableManager::IsRerenderingRequired(
    vtkMRMLProjectiveTextureMappingDisplayNode *displayNode) {
    return displayNode->GetVisibility() && this->IsDisplayNodeValid(displayNode);
}

bool
vtkMRMLProjectiveTextureMappingDisplayableManager::IsDisplayNodeValid(
    vtkMRMLProjectiveTextureMappingDisplayNode *node) const {
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
