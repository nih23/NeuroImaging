#include <QDebug>

#include "qSlicerImageFusionModuleWidget.h"
#include "ui_qSlicerImageFusionModuleWidget.h"

#include <vtkMRMLVolumeRenderingDisplayNode.h>

#include "vtkMRMLImageFusionDisplayNode.h"
#include "vtkMRMLProjectiveTextureMappingDisplayNode.h"

class qSlicerImageFusionModuleWidgetPrivate : public Ui_qSlicerImageFusionModuleWidget {
    Q_DECLARE_PUBLIC(qSlicerImageFusionModuleWidget);

public:
    qSlicerImageFusionModuleWidgetPrivate(qSlicerImageFusionModuleWidget &object);
    virtual ~qSlicerImageFusionModuleWidgetPrivate();

    void
    setupUi(qSlicerImageFusionModuleWidget *q);

    vtkMRMLImageFusionDisplayNode *DisplayNode;

protected:
    qSlicerImageFusionModuleWidget *const q_ptr;
};

qSlicerImageFusionModuleWidgetPrivate::qSlicerImageFusionModuleWidgetPrivate(
    qSlicerImageFusionModuleWidget &object)
    : DisplayNode{nullptr}, q_ptr{&object} {}

qSlicerImageFusionModuleWidgetPrivate::~qSlicerImageFusionModuleWidgetPrivate() {}

void
qSlicerImageFusionModuleWidgetPrivate::setupUi(qSlicerImageFusionModuleWidget *q) {
    this->Ui_qSlicerImageFusionModuleWidget::setupUi(q);

    QObject::connect(this->ImageFusionDisplayNodeComboBox,
                     static_cast<void (qMRMLNodeComboBox::*)(vtkMRMLNode *)>(
                         &qMRMLNodeComboBox::currentNodeChanged),
                     q,
                     &qSlicerImageFusionModuleWidget::onCurrentMRMLImageFusionDisplayNodeChanged);
    QObject::connect(
        this->VolumeRenderingDisplayNodeComboBox,
        static_cast<void (qMRMLNodeComboBox::*)(vtkMRMLNode *)>(
            &qMRMLNodeComboBox::currentNodeChanged),
        q,
        &qSlicerImageFusionModuleWidget::onCurrentMRMLVolumeRenderingDisplayNodeChanged);
    QObject::connect(
        this->ProjectiveTextureMappingDisplayNodeComboBox,
        static_cast<void (qMRMLNodeComboBox::*)(vtkMRMLNode *)>(
            &qMRMLNodeComboBox::currentNodeChanged),
        q,
        &qSlicerImageFusionModuleWidget::onCurrentMRMLProjectiveTextureMappingDisplayNodeChanged);
    QObject::connect(this->WeightedAveragingRadioButton,
                     &QRadioButton::toggled,
                     q,
                     &qSlicerImageFusionModuleWidget::onWeightedAveragingToggled);
    QObject::connect(this->GuidedFilteringRadioButton,
                     &QRadioButton::toggled,
                     q,
                     &qSlicerImageFusionModuleWidget::onGuidedFilteringToggled);
    QObject::connect(this->VolumeRenderingWeightSliderWidget,
                     &ctkSliderWidget::valueChanged,
                     q,
                     &qSlicerImageFusionModuleWidget::onVolumeRenderingWeightChanged);
}

qSlicerImageFusionModuleWidget::qSlicerImageFusionModuleWidget(QWidget *_parent)
    : Superclass(_parent), d_ptr(new qSlicerImageFusionModuleWidgetPrivate(*this)) {}

qSlicerImageFusionModuleWidget::~qSlicerImageFusionModuleWidget() {
    Q_D(qSlicerImageFusionModuleWidget);
    if (d->DisplayNode) {
        this->qvtkDisconnect(d->DisplayNode,
                             vtkCommand::ModifiedEvent,
                             this,
                             SLOT(updateFromMRMLImageFusionDisplayNode()));
    }

    d->ImageFusionDisplayNodeComboBox->setCurrentNode(nullptr);
}

bool
qSlicerImageFusionModuleWidget::setEditedNode(vtkMRMLNode *node, QString role, QString context) {
    Q_D(qSlicerImageFusionModuleWidget);
    Q_UNUSED(role);
    Q_UNUSED(context);

    auto imageFusionDisplayNode = vtkMRMLImageFusionDisplayNode::SafeDownCast(node);
    if (imageFusionDisplayNode) {
        d->ImageFusionDisplayNodeComboBox->setCurrentNode(imageFusionDisplayNode);
        return true;
    }

    return false;
}

double
qSlicerImageFusionModuleWidget::nodeEditable(vtkMRMLNode *node) {
    if (vtkMRMLImageFusionDisplayNode::SafeDownCast(node)) {
        return 0.5;
    }

    return 0.0;
}

void
qSlicerImageFusionModuleWidget::onCurrentMRMLImageFusionDisplayNodeChanged(vtkMRMLNode *node) {
    Q_D(qSlicerImageFusionModuleWidget);

    auto imageFusionDisplayNode = vtkMRMLImageFusionDisplayNode::SafeDownCast(node);

    this->qvtkReconnect(d->DisplayNode,
                        imageFusionDisplayNode,
                        vtkCommand::ModifiedEvent,
                        this,
                        SLOT(updateFromMRMLImageFusionDisplayNode()));

    d->DisplayNode = imageFusionDisplayNode;

    this->updateFromMRMLImageFusionDisplayNode();
}

void
qSlicerImageFusionModuleWidget::onCurrentMRMLVolumeRenderingDisplayNodeChanged(vtkMRMLNode *node) {
    Q_D(qSlicerImageFusionModuleWidget);
    if (d->DisplayNode) {
        d->DisplayNode->SetAndObserveVolumeRenderingDisplayNodeID(node ? node->GetID() : nullptr);
    }
}

void
qSlicerImageFusionModuleWidget::onCurrentMRMLProjectiveTextureMappingDisplayNodeChanged(
    vtkMRMLNode *node) {
    Q_D(qSlicerImageFusionModuleWidget);
    if (d->DisplayNode) {
        d->DisplayNode->SetAndObserveProjectiveTextureMappingDisplayNodeID(node ? node->GetID()
                                                                                : nullptr);
    }
}

void
qSlicerImageFusionModuleWidget::onWeightedAveragingToggled(bool status) {
    Q_D(qSlicerImageFusionModuleWidget);
    if (d->DisplayNode && status) {
        d->DisplayNode->SetImageFusionMethodToWeightedAveraging();
    }
}

void
qSlicerImageFusionModuleWidget::onGuidedFilteringToggled(bool status) {
    Q_D(qSlicerImageFusionModuleWidget);
    if (d->DisplayNode && status) {
        d->DisplayNode->SetImageFusionMethodToGuidedFiltering();
    }
}

void
qSlicerImageFusionModuleWidget::onVolumeRenderingWeightChanged(double weight) {
    Q_D(qSlicerImageFusionModuleWidget);
    if (d->DisplayNode) {
        d->DisplayNode->SetVolumeRenderingWeight(weight);
    }
}


void
qSlicerImageFusionModuleWidget::updateFromMRMLImageFusionDisplayNode() {
    Q_D(qSlicerImageFusionModuleWidget);
    d->VolumeRenderingDisplayNodeComboBox->setCurrentNode(
        d->DisplayNode ? d->DisplayNode->GetVolumeRenderingDisplayNode() : nullptr);
    d->ProjectiveTextureMappingDisplayNodeComboBox->setCurrentNode(
        d->DisplayNode ? d->DisplayNode->GetProjectiveTextureMappingDisplayNode() : nullptr);
    d->WeightedAveragingRadioButton->setChecked(
        d->DisplayNode ? d->DisplayNode->GetImageFusionMethod() ==
                vtkMRMLImageFusionDisplayNode::WEIGHTED_AVERAGING
                       : true);
    d->GuidedFilteringRadioButton->setChecked(
        d->DisplayNode ? d->DisplayNode->GetImageFusionMethod() ==
                vtkMRMLImageFusionDisplayNode::GUIDED_FILTERING
                       : false);
    d->VolumeRenderingWeightSliderWidget->setValue(
        d->DisplayNode ? d->DisplayNode->GetVolumeRenderingWeight() : 0.5);
}

void
qSlicerImageFusionModuleWidget::setup() {
    Q_D(qSlicerImageFusionModuleWidget);
    d->setupUi(this);
    this->Superclass::setup();
}
