#include <QDebug>

#include "qSlicerProjectiveTextureMappingModuleWidget.h"
#include "ui_qSlicerProjectiveTextureMappingModuleWidget.h"

#include <vtkMRMLAbstractViewNode.h>
#include <vtkMRMLCameraNode.h>
#include <vtkMRMLColorNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLVectorVolumeNode.h>
#include <vtkMRMLVolumeNode.h>

#include "vtkMRMLProjectiveTextureMappingDisplayNode.h"

#include "vtkSlicerProjectiveTextureMappingLogic.h"

class qSlicerProjectiveTextureMappingModuleWidgetPrivate
    : public Ui_qSlicerProjectiveTextureMappingModuleWidget {
    Q_DECLARE_PUBLIC(qSlicerProjectiveTextureMappingModuleWidget);

public:
    qSlicerProjectiveTextureMappingModuleWidgetPrivate(
        qSlicerProjectiveTextureMappingModuleWidget &object);
    virtual ~qSlicerProjectiveTextureMappingModuleWidgetPrivate();

    void
    setupUi(qSlicerProjectiveTextureMappingModuleWidget *q);

    vtkMRMLProjectiveTextureMappingDisplayNode *DisplayNode;

protected:
    qSlicerProjectiveTextureMappingModuleWidget *const q_ptr;
};

qSlicerProjectiveTextureMappingModuleWidgetPrivate::
    qSlicerProjectiveTextureMappingModuleWidgetPrivate(
        qSlicerProjectiveTextureMappingModuleWidget &object)
    : DisplayNode{nullptr}, q_ptr(&object) {}

qSlicerProjectiveTextureMappingModuleWidgetPrivate::
    ~qSlicerProjectiveTextureMappingModuleWidgetPrivate() {}

void
qSlicerProjectiveTextureMappingModuleWidgetPrivate::setupUi(
    qSlicerProjectiveTextureMappingModuleWidget *q) {
    this->Ui_qSlicerProjectiveTextureMappingModuleWidget::setupUi(q);

    QObject::connect(
        this->SurfaceModelNodeComboBox,
        static_cast<void (qMRMLNodeComboBox::*)(vtkMRMLNode *)>(
            &qMRMLNodeComboBox::currentNodeChanged),
        q,
        &qSlicerProjectiveTextureMappingModuleWidget::onCurrentMRMLSurfaceModelNodeChanged);

    QObject::connect(this->VisibilityCheckBox,
                     &ctkCheckBox::toggled,
                     q,
                     &qSlicerProjectiveTextureMappingModuleWidget::onVisibilityChanged);

    QObject::connect(this->DisplayNodeComboBox,
                     static_cast<void (qMRMLNodeComboBox::*)(vtkMRMLNode *)>(
                         &qMRMLNodeComboBox::currentNodeChanged),
                     q,
                     &qSlicerProjectiveTextureMappingModuleWidget::onCurrentMRMLDisplayNodeChanged);

    QObject::connect(this->TextureNodeComboBox,
                     static_cast<void (qMRMLNodeComboBox::*)(vtkMRMLNode *)>(
                         &qMRMLNodeComboBox::currentNodeChanged),
                     q,
                     &qSlicerProjectiveTextureMappingModuleWidget::onCurrentMRMLTextureNodeChanged);

    QObject::connect(
        this->ColorMapNodeComboBox,
        static_cast<void (qMRMLNodeComboBox::*)(vtkMRMLNode *)>(
            &qMRMLNodeComboBox::currentNodeChanged),
        q,
        &qSlicerProjectiveTextureMappingModuleWidget::onCurrentMRMLColorMapNodeChanged);

    QObject::connect(this->CameraNodeComboBox,
                     static_cast<void (qMRMLNodeComboBox::*)(vtkMRMLNode *)>(
                         &qMRMLNodeComboBox::currentNodeChanged),
                     q,
                     &qSlicerProjectiveTextureMappingModuleWidget::onCurrentMRMLCameraNodeChanged);

    QObject::connect(this->ZToleranceSliderWidget,
                     &ctkSliderWidget::valueChanged,
                     q,
                     &qSlicerProjectiveTextureMappingModuleWidget::onZToleranceChanged);
}

qSlicerProjectiveTextureMappingModuleWidget::qSlicerProjectiveTextureMappingModuleWidget(
    QWidget *_parent)
    : Superclass(_parent), d_ptr(new qSlicerProjectiveTextureMappingModuleWidgetPrivate(*this)) {}

qSlicerProjectiveTextureMappingModuleWidget::~qSlicerProjectiveTextureMappingModuleWidget() {
    Q_D(qSlicerProjectiveTextureMappingModuleWidget);
    if (d->DisplayNode) {
        this->qvtkDisconnect(
            d->DisplayNode, vtkCommand::ModifiedEvent, this, SLOT(updateFromMRMLProjectiveTextureMappingDisplayNode()));
    }

    d->DisplayNodeComboBox->setCurrentNode(nullptr);
}

bool
qSlicerProjectiveTextureMappingModuleWidget::setEditedNode(vtkMRMLNode *node,
                                                           QString role,
                                                           QString context) {
    Q_D(qSlicerProjectiveTextureMappingModuleWidget);
    Q_UNUSED(role);
    Q_UNUSED(context);

    auto displayNode = vtkMRMLProjectiveTextureMappingDisplayNode::SafeDownCast(node);
    if (displayNode) {
        d->DisplayNodeComboBox->setCurrentNode(displayNode);
        return true;
    }

    return false;
}

double
qSlicerProjectiveTextureMappingModuleWidget::nodeEditable(vtkMRMLNode *node) {
    if (vtkMRMLProjectiveTextureMappingDisplayNode::SafeDownCast(node)) {
        return 0.5;
    }

    return 0.0;
}

void
qSlicerProjectiveTextureMappingModuleWidget::onCurrentMRMLDisplayNodeChanged(vtkMRMLNode *node) {
    Q_D(qSlicerProjectiveTextureMappingModuleWidget);

    auto displayNode = vtkMRMLProjectiveTextureMappingDisplayNode::SafeDownCast(node);

    this->qvtkReconnect(d->DisplayNode,
                        displayNode,
                        vtkCommand::ModifiedEvent,
                        this,
                        SLOT(updateFromMRMLProjectiveTextureMappingDisplayNode()));

    d->DisplayNode = displayNode;

    this->updateFromMRMLProjectiveTextureMappingDisplayNode();
}

void
qSlicerProjectiveTextureMappingModuleWidget::onCurrentMRMLSurfaceModelNodeChanged(
    vtkMRMLNode *node) {
    Q_D(qSlicerProjectiveTextureMappingModuleWidget);
    if (d->DisplayNode) {
        d->DisplayNode->SetAndObserveSurfaceModelNodeID(node ? node->GetID() : nullptr);
    }
}

void
qSlicerProjectiveTextureMappingModuleWidget::onCurrentMRMLTextureNodeChanged(vtkMRMLNode *node) {
    Q_D(qSlicerProjectiveTextureMappingModuleWidget);
    if (d->DisplayNode) {
        d->DisplayNode->SetAndObserveTextureNodeID(node ? node->GetID() : nullptr);
    }
}

void
qSlicerProjectiveTextureMappingModuleWidget::onCurrentMRMLColorMapNodeChanged(vtkMRMLNode *node) {
    Q_D(qSlicerProjectiveTextureMappingModuleWidget);
    if (d->DisplayNode) {
        d->DisplayNode->SetAndObserveColorMapNodeID(node ? node->GetID() : nullptr);
    }
}

void
qSlicerProjectiveTextureMappingModuleWidget::onCurrentMRMLCameraNodeChanged(vtkMRMLNode *node) {
    Q_D(qSlicerProjectiveTextureMappingModuleWidget);
    if (d->DisplayNode) {
        d->DisplayNode->SetAndObserveCameraNodeID(node ? node->GetID() : nullptr);
    }
}

void
qSlicerProjectiveTextureMappingModuleWidget::onZToleranceChanged(double zTolerance) {
    Q_D(qSlicerProjectiveTextureMappingModuleWidget);
    if (d->DisplayNode) {
        d->DisplayNode->SetZTolerance(zTolerance);
    }
}

void
qSlicerProjectiveTextureMappingModuleWidget::onVisibilityChanged(bool visible) {
    Q_D(qSlicerProjectiveTextureMappingModuleWidget);
    if (d->DisplayNode) {
        d->DisplayNode->SetVisibility(visible);
    }
}

void
qSlicerProjectiveTextureMappingModuleWidget::updateFromMRMLProjectiveTextureMappingDisplayNode() {
    Q_D(qSlicerProjectiveTextureMappingModuleWidget);
    d->SurfaceModelNodeComboBox->setCurrentNode(d->DisplayNode ? d->DisplayNode->GetSurfaceModelNode() : nullptr);
    d->TextureNodeComboBox->setCurrentNode(d->DisplayNode ? d->DisplayNode->GetTextureNode() : nullptr);
    d->ColorMapNodeComboBox->setCurrentNode(d->DisplayNode ? d->DisplayNode->GetColorMapNode() : nullptr);
    d->CameraNodeComboBox->setCurrentNode(d->DisplayNode ? d->DisplayNode->GetCameraNode() : nullptr);
    d->ZToleranceSliderWidget->setValue(d->DisplayNode ? d->DisplayNode->GetZTolerance() : 0.0);
    d->VisibilityCheckBox->setChecked(d->DisplayNode ? d->DisplayNode->GetVisibility() : false);
}

void
qSlicerProjectiveTextureMappingModuleWidget::setup() {
    Q_D(qSlicerProjectiveTextureMappingModuleWidget);
    d->setupUi(this);
    this->Superclass::setup();
}
