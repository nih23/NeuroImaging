#include "qSlicerCameraCreatorModuleWidget.h"

#include <QDebug>

#include "ui_qSlicerCameraCreatorModuleWidget.h"

#include "vtkSlicerCameraCreatorLogic.h"

class qSlicerCameraCreatorModuleWidgetPrivate : public Ui_qSlicerCameraCreatorModuleWidget {
    Q_DECLARE_PUBLIC(qSlicerCameraCreatorModuleWidget);

public:
    qSlicerCameraCreatorModuleWidgetPrivate(qSlicerCameraCreatorModuleWidget &object);

    virtual ~qSlicerCameraCreatorModuleWidgetPrivate();

    void
    setupUi(qSlicerCameraCreatorModuleWidget *q);

    vtkSlicerCameraCreatorLogic *Logic;

protected:
    qSlicerCameraCreatorModuleWidget *const q_ptr;
};

qSlicerCameraCreatorModuleWidgetPrivate::qSlicerCameraCreatorModuleWidgetPrivate(
    qSlicerCameraCreatorModuleWidget &object)
    : Logic{nullptr}, q_ptr{&object} {}

qSlicerCameraCreatorModuleWidgetPrivate::~qSlicerCameraCreatorModuleWidgetPrivate() {
    this->Logic = nullptr;
}

void
qSlicerCameraCreatorModuleWidgetPrivate::setupUi(qSlicerCameraCreatorModuleWidget *q) {
    this->Ui_qSlicerCameraCreatorModuleWidget::setupUi(q);
    QObject::connect(this->CameraNodeComboBox,
                     static_cast<void (qMRMLNodeComboBox::*)(vtkMRMLNode *)>(
                         &qMRMLNodeComboBox::currentNodeChanged),
                     q,
                     &qSlicerCameraCreatorModuleWidget::onCameraNodeChanged);

    QObject::connect(this->ReferenceToRASTransformNodeComboBox,
                     static_cast<void (qMRMLNodeComboBox::*)(vtkMRMLNode *)>(
                         &qMRMLNodeComboBox::currentNodeChanged),
                     q,
                     &qSlicerCameraCreatorModuleWidget::onReferenceToRASTransformNodeChanged);

    QObject::connect(this->CameraToReferenceTransformNodeComboBox,
                     static_cast<void (qMRMLNodeComboBox::*)(vtkMRMLNode *)>(
                         &qMRMLNodeComboBox::currentNodeChanged),
                     q,
                     &qSlicerCameraCreatorModuleWidget::onCameraToReferenceTransformNodeChanged);

    QObject::connect(
        this->ViewUpXSpinBox,
        static_cast<void (ctkDoubleSpinBox::*)(double)>(&ctkDoubleSpinBox::valueChanged),
        q,
        &qSlicerCameraCreatorModuleWidget::onViewUpChanged);
    QObject::connect(
        this->ViewUpYSpinBox,
        static_cast<void (ctkDoubleSpinBox::*)(double)>(&ctkDoubleSpinBox::valueChanged),
        q,
        &qSlicerCameraCreatorModuleWidget::onViewUpChanged);
    QObject::connect(
        this->ViewUpZSpinBox,
        static_cast<void (ctkDoubleSpinBox::*)(double)>(&ctkDoubleSpinBox::valueChanged),
        q,
        &qSlicerCameraCreatorModuleWidget::onViewUpChanged);

    QObject::connect(
        this->ViewDirectionXSpinBox,
        static_cast<void (ctkDoubleSpinBox::*)(double)>(&ctkDoubleSpinBox::valueChanged),
        q,
        &qSlicerCameraCreatorModuleWidget::onViewDirectionChanged);
    QObject::connect(
        this->ViewDirectionYSpinBox,
        static_cast<void (ctkDoubleSpinBox::*)(double)>(&ctkDoubleSpinBox::valueChanged),
        q,
        &qSlicerCameraCreatorModuleWidget::onViewDirectionChanged);
    QObject::connect(
        this->ViewDirectionZSpinBox,
        static_cast<void (ctkDoubleSpinBox::*)(double)>(&ctkDoubleSpinBox::valueChanged),
        q,
        &qSlicerCameraCreatorModuleWidget::onViewDirectionChanged);

    QObject::connect(
        this->ViewAngleSpinBox,
        static_cast<void (ctkDoubleSpinBox::*)(double)>(&ctkDoubleSpinBox::valueChanged),
        q,
        &qSlicerCameraCreatorModuleWidget::onViewAngleChanged);

    QObject::connect(
        this->ApplyButton, &QPushButton::pressed, q, &qSlicerCameraCreatorModuleWidget::onApplied);

    q->onViewUpChanged();
    q->onViewDirectionChanged();
    q->onViewAngleChanged(this->ViewAngleSpinBox->value());
}

qSlicerCameraCreatorModuleWidget::qSlicerCameraCreatorModuleWidget(QWidget *_parent)
    : Superclass(_parent), d_ptr(new qSlicerCameraCreatorModuleWidgetPrivate(*this)) {}

qSlicerCameraCreatorModuleWidget::~qSlicerCameraCreatorModuleWidget() {}

void
qSlicerCameraCreatorModuleWidget::onCameraNodeChanged(vtkMRMLNode *node) {
    Q_D(qSlicerCameraCreatorModuleWidget);
    if (d->Logic) {
        d->Logic->SetCameraNode(node);
    }
}

void
qSlicerCameraCreatorModuleWidget::onReferenceToRASTransformNodeChanged(vtkMRMLNode *node) {
    Q_D(qSlicerCameraCreatorModuleWidget);
    if (d->Logic) {
        d->Logic->SetReferenceToRASTransformNode(node);
    }
}

void
qSlicerCameraCreatorModuleWidget::onCameraToReferenceTransformNodeChanged(vtkMRMLNode *node) {
    Q_D(qSlicerCameraCreatorModuleWidget);
    if (d->Logic) {
        d->Logic->SetCameraToReferenceTransformNode(node);
    }
}

void
qSlicerCameraCreatorModuleWidget::onViewUpChanged() {
    Q_D(qSlicerCameraCreatorModuleWidget);
    double viewUp[3];
    viewUp[0] = d->ViewUpXSpinBox->value();
    viewUp[1] = d->ViewUpYSpinBox->value();
    viewUp[2] = d->ViewUpZSpinBox->value();
    if (d->Logic) {
        d->Logic->SetViewUp(viewUp);
    }
}

void
qSlicerCameraCreatorModuleWidget::onViewDirectionChanged() {
    Q_D(qSlicerCameraCreatorModuleWidget);
    double viewDirection[3];
    viewDirection[0] = d->ViewDirectionXSpinBox->value();
    viewDirection[1] = d->ViewDirectionYSpinBox->value();
    viewDirection[2] = d->ViewDirectionZSpinBox->value();
    if (d->Logic) {
        d->Logic->SetViewDirection(viewDirection);
    }
}

void
qSlicerCameraCreatorModuleWidget::onViewAngleChanged(double value) {
    Q_D(qSlicerCameraCreatorModuleWidget);
    if (d->Logic) {
        d->Logic->SetViewAngle(value);
    }
}

void
qSlicerCameraCreatorModuleWidget::onApplied() {
    Q_D(qSlicerCameraCreatorModuleWidget);
    if (d->Logic) {
        d->Logic->UpdateCamera();
    }
}

void
qSlicerCameraCreatorModuleWidget::setup() {
    Q_D(qSlicerCameraCreatorModuleWidget);
    d->Logic = vtkSlicerCameraCreatorLogic::SafeDownCast(this->logic());
    d->setupUi(this);
    this->Superclass::setup();
}
