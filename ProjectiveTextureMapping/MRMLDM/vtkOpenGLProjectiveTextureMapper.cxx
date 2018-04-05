#include "vtkOpenGLProjectiveTextureMapper.h"

#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkOpenGLActor.h>
#include <vtkOpenGLCamera.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkOpenGLResourceFreeCallback.h>
#include <vtkOpenGLVertexBufferObject.h>
#include <vtkOpenGLVertexBufferObjectGroup.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkShaderProgram.h>
#include <vtkWindow.h>

vtkStandardNewMacro(vtkOpenGLProjectiveTextureMapper);

void
vtkOpenGLProjectiveTextureMapper::PrintSelf(std::ostream &os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);
    os << indent << "ZTolerance : " << this->ZTolerance << endl;
    os << indent << "AspectRatio : " << this->AspectRatio << endl;
    if (this->DepthRenderer) {
        os << indent << "DepthRenderer :" << endl;
        this->DepthRenderer->PrintSelf(os, indent.GetNextIndent());
    } else {
        os << indent << "DepthRenderer : (None)" << endl;
    }

    os << indent << "ProjectedTexture :" << endl;
    ProjectedTexture->PrintSelf(os, indent.GetNextIndent());
    os << indent << "DepthTexture :" << endl;
    DepthTexture->PrintSelf(os, indent.GetNextIndent());
}

void
vtkOpenGLProjectiveTextureMapper::RenderPieceStart(vtkRenderer *ren, vtkActor *act) {
    this->Superclass::RenderPieceStart(ren, act);
    this->ProjectedTexture->Update();
    this->ProjectedTexture->Load(ren);
    this->DepthTexture->Update();
    this->DepthTexture->Load(ren);
}

void
vtkOpenGLProjectiveTextureMapper::RenderPieceFinish(vtkRenderer *ren, vtkActor *act) {
    this->DepthTexture->PostRender(ren);
    this->ProjectedTexture->PostRender(ren);
    this->Superclass::RenderPieceFinish(ren, act);
}

void
vtkOpenGLProjectiveTextureMapper::ReleaseGraphicsResources(vtkWindow *win) {
    if (!this->ResourceCallback->IsReleasing()) {
        this->ResourceCallback->Release();
        return;
    }

    this->DepthTexture->ReleaseGraphicsResources(win);
    this->ProjectedTexture->ReleaseGraphicsResources(win);
    this->Superclass::ReleaseGraphicsResources(win);
}

void
vtkOpenGLProjectiveTextureMapper::SetDepthRenderer(vtkRenderer *renderer) {
    if (renderer == this->DepthRenderer) {
        return;
    }

    if (this->DepthRenderer) {
        this->DepthRenderer->UnRegister(this);
    }
    this->DepthRenderer = renderer;
    if (this->DepthRenderer) {
        this->DepthRenderer->Register(this);
    }
    this->Modified();
}

void
vtkOpenGLProjectiveTextureMapper::SetProjectedTextureConnection(vtkAlgorithmOutput *input) {
    this->MappedProjectedTexture->SetInputConnection(nullptr);
    this->ProjectedTexture->SetInputConnection(input);
}

void
vtkOpenGLProjectiveTextureMapper::SetProjectedTextureConnection(vtkAlgorithmOutput *input,
                                                                vtkScalarsToColors *colorMap) {
    this->MappedProjectedTexture->SetInputConnection(input);
    this->MappedProjectedTexture->SetLookupTable(colorMap);
    this->ProjectedTexture->SetInputConnection(this->MappedProjectedTexture->GetOutputPort());
}

vtkAlgorithmOutput *
vtkOpenGLProjectiveTextureMapper::GetProjectedTextureConnection() {
    if (this->MappedProjectedTexture->GetNumberOfInputConnections(0) == 1) {
        return this->MappedProjectedTexture->GetInputConnection(0, 0);
    } else if (this->ProjectedTexture->GetNumberOfInputConnections(0) == 1) {
        return this->ProjectedTexture->GetInputConnection(0, 0);
    }
    return nullptr;
}

void
vtkOpenGLProjectiveTextureMapper::SetDepthConnection(vtkAlgorithmOutput *input) {
    this->DepthTexture->SetInputConnection(input);
}

vtkAlgorithmOutput *
vtkOpenGLProjectiveTextureMapper::GetDepthConnection() {
    return this->DepthTexture->GetInputConnection(0, 0);
}

vtkOpenGLProjectiveTextureMapper::vtkOpenGLProjectiveTextureMapper() {
    this->DepthRenderer = nullptr;
    this->ZTolerance = 0.0f;
    this->AspectRatio = 0.0f;

    this->DepthTexture->RepeatOff();
    this->DepthTexture->EdgeClampOff();
    this->DepthTexture->InterpolateOn();

    this->MappedProjectedTexture->SetOutputFormatToRGB();

    this->ProjectedTexture->RepeatOff();
    this->ProjectedTexture->EdgeClampOff();
    this->ProjectedTexture->InterpolateOff();

    if (this->ResourceCallback) {
        this->ResourceCallback->Release();
        delete this->ResourceCallback;
        this->ResourceCallback = nullptr;
    }
    this->ResourceCallback = new vtkOpenGLResourceFreeCallback<vtkOpenGLProjectiveTextureMapper>(
        this, &vtkOpenGLProjectiveTextureMapper::ReleaseGraphicsResources);
}

vtkOpenGLProjectiveTextureMapper::~vtkOpenGLProjectiveTextureMapper() {
    if (this->DepthRenderer) {
        this->DepthRenderer->UnRegister(this);
    }
}

void
vtkOpenGLProjectiveTextureMapper::ReplaceShaderTCoord(
    std::map<vtkShader::Type, vtkShader *> shaders, vtkRenderer *ren, vtkActor *act) {
    std::string FSSource = shaders[vtkShader::Fragment]->GetSource();

    vtkShaderProgram::Substitute(FSSource,
                                 "//VTK::TCoord::Dec",
                                 "uniform mat4 WCVCMatrix;\n"
                                 "uniform float cameraZNear;\n"
                                 "uniform float cameraZFar;\n"
                                 "uniform vec3 cameraOrientationWC;\n"
                                 "uniform vec3 cameraPositionWC;\n"
                                 "uniform vec3 cameraRightWC;\n"
                                 "uniform vec3 cameraUpWC;\n"
                                 "uniform vec2 cameraImagePlaneSize;\n"
                                 "uniform sampler2D cameraImage;\n"
                                 "uniform sampler2D cameraDepth;\n"
                                 "uniform float zTolerance;");

    vtkShaderProgram::Substitute(
        FSSource,
        "//VTK::TCoord::Impl",
        "vec3 cameraOrientationVC = normalize(WCVCMatrix * vec4(cameraOrientationWC, 0)).xyz;\n"
        "  float normalOrientationCosinus = dot(cameraOrientationVC, ptmNormalVCVSOutput);\n"
        "  if (normalOrientationCosinus > 0.0) { discard; }\n"
        "  vec3 cameraPositionVC = (WCVCMatrix * vec4(cameraPositionWC, 1)).xyz;\n"
        "  vec3 diffVC = ptmVertexVCVSOutput.xyz - cameraPositionVC;\n"
        "  float viewZ = dot(diffVC, cameraOrientationVC);\n"
        "  diffVC = diffVC / viewZ - cameraOrientationVC;\n"
        "  vec2 tcoord;\n"
        "  vec3 cameraRightVC = normalize(WCVCMatrix * vec4(cameraRightWC, 0)).xyz;\n"
        "  vec3 cameraUpVC = normalize(WCVCMatrix * vec4(cameraUpWC, 0)).xyz;\n"
        "  tcoord.s = dot(diffVC, cameraRightVC) / cameraImagePlaneSize.s + 0.5;\n"
        "  tcoord.t = dot(diffVC, cameraUpVC) / cameraImagePlaneSize.t + 0.5;\n"
        "  if (tcoord.s < 0.0 || tcoord.s > 1.0 || tcoord.t < 0.0 || tcoord.s > 1.0) { discard; }\n"
        "  float bufferValue = texture(cameraDepth, tcoord).r;\n"
        "  float bufferZ = 1.0 / ((bufferValue - 0.5 - (cameraZFar + cameraZNear) / (2.0 * ("
        "cameraZFar - cameraZNear))) * (cameraZFar - cameraZNear) / (-cameraZFar * cameraZNear));\n"
        "  if (viewZ > bufferZ + zTolerance) { discard; }\n"
        "  tcoord.t = 1 - tcoord.t;\n"
        "  vec4 color = texture(cameraImage, tcoord);\n"
        "  gl_FragData[0] = color;");
//        "  gl_FragData[0].a *= smoothstep(0, 1, -normalOrientationCosinus);");

    shaders[vtkShader::Fragment]->SetSource(FSSource);

    this->Superclass::ReplaceShaderTCoord(shaders, ren, act);
}

void
vtkOpenGLProjectiveTextureMapper::ReplaceShaderNormal(
    std::map<vtkShader::Type, vtkShader *> shaders, vtkRenderer *ren, vtkActor *act) {
    std::string VSSource = shaders[vtkShader::Vertex]->GetSource();
    std::string FSSource = shaders[vtkShader::Fragment]->GetSource();

    vtkShaderProgram::Substitute(VSSource,
                                 "//VTK::Normal::Dec",
                                 "//VTK::Normal::Dec\n"
                                 "attribute vec3 normalMC;\n"
                                 "varying vec3 ptmNormalVCVSOutput;");
    if (this->LastLightComplexity[this->LastBoundBO] <= 0) {
        vtkShaderProgram::Substitute(VSSource,
                                     "//VTK::Normal::Dec",
                                     "//VTK::Normal::Dec\n"
                                     "uniform mat3 normalMatrix;");
    }

    vtkShaderProgram::Substitute(VSSource,
                                 "//VTK::Normal::Impl",
                                 "//VTK::Normal::Impl\n"
                                 "  ptmNormalVCVSOutput = normalMatrix * normalMC;");

    vtkShaderProgram::Substitute(FSSource,
                                 "//VTK::Normal::Dec",
                                 "//VTK::Normal::Dec\n"
                                 "varying vec3 ptmNormalVCVSOutput;");

    vtkShaderProgram::Substitute(FSSource,
                                 "//VTK::Normal::Impl",
                                 "//VTK::Normal::Impl\n"
                                 "  vec3 ptmNormalVCVSOutput = normalize(ptmNormalVCVSOutput);");

    shaders[vtkShader::Vertex]->SetSource(VSSource);
    shaders[vtkShader::Fragment]->SetSource(FSSource);

    this->Superclass::ReplaceShaderNormal(shaders, ren, act);
}

void
vtkOpenGLProjectiveTextureMapper::ReplaceShaderPositionVC(
    std::map<vtkShader::Type, vtkShader *> shaders, vtkRenderer *ren, vtkActor *act) {
    std::string VSSource = shaders[vtkShader::Vertex]->GetSource();
    std::string FSSource = shaders[vtkShader::Fragment]->GetSource();

    vtkShaderProgram::Substitute(VSSource,
                                 "//VTK::Camera::Dec",
                                 "uniform mat4 MCDCMatrix;\n"
                                 "uniform mat4 MCVCMatrix;");

    vtkShaderProgram::Substitute(VSSource,
                                 "//VTK::PositionVC::Dec",
                                 "//VTK::PositionVC::Dec\n"
                                 "varying vec4 ptmVertexVCVSOutput;");

    vtkShaderProgram::Substitute(VSSource,
                                 "//VTK::PositionVC::Impl",
                                 "//VTK::PositionVC::Impl\n"
                                 "  ptmVertexVCVSOutput = MCVCMatrix * vertexMC;");

    vtkShaderProgram::Substitute(FSSource,
                                 "//VTK::PositionVC::Dec",
                                 "//VTK::PositionVC::Dec\n"
                                 "varying vec4 ptmVertexVCVSOutput;");

    shaders[vtkShader::Vertex]->SetSource(VSSource);
    shaders[vtkShader::Fragment]->SetSource(FSSource);

    this->Superclass::ReplaceShaderPositionVC(shaders, ren, act);
}

void
vtkOpenGLProjectiveTextureMapper::SetCameraShaderParameters(vtkOpenGLHelper &cellBO,
                                                            vtkRenderer *ren,
                                                            vtkActor *act) {
    this->Superclass::SetCameraShaderParameters(cellBO, ren, act);

    vtkShaderProgram *program = cellBO.Program;

    auto cam = dynamic_cast<vtkOpenGLCamera *>(ren->GetActiveCamera());
    assert(cam);
    vtkMatrix4x4 *wcdc;
    vtkMatrix4x4 *wcvc;
    vtkMatrix3x3 *norms;
    vtkMatrix4x4 *vcdc;
    cam->GetKeyMatrices(ren, wcvc, norms, vcdc, wcdc);

    assert(wcvc);
    bool ok;
    ok = program->SetUniformMatrix("WCVCMatrix", wcvc);
    if (!ok) {
        vtkErrorMacro(<< "Couldn't set WCVCMatrix");
    }

    assert(DepthRenderer);
    if (DepthRenderer) {
        double cameraOrientation[3], cameraPosition[3];
        DepthRenderer->GetActiveCamera()->GetPosition(cameraPosition);
        DepthRenderer->GetActiveCamera()->GetFocalPoint(cameraOrientation);
        for (int i = 0; i < 3; ++i) {
            cameraOrientation[i] -= cameraPosition[i];
        }

        float cameraOrientationWC[3], cameraPositionWC[3];
        for (int i = 0; i < 3; ++i) {
            cameraOrientationWC[i] = static_cast<float>(cameraOrientation[i]);
            cameraPositionWC[i] = static_cast<float>(cameraPosition[i]);
        }

        double cameraZNear, cameraZFar;
        DepthRenderer->GetActiveCamera()->GetClippingRange(cameraZNear, cameraZFar);

        double rightv[3], upv[3];
        vtkMath::Cross(cameraOrientation, DepthRenderer->GetActiveCamera()->GetViewUp(), rightv);
        vtkMath::Normalize(rightv);

        vtkMath::Cross(rightv, cameraOrientation, upv);
        vtkMath::Normalize(upv);

        float cameraRightWC[3], cameraUpWC[3];
        for (int i = 0; i < 3; ++i) {
            cameraRightWC[i] = static_cast<float>(rightv[i]);
            cameraUpWC[i] = static_cast<float>(upv[i]);
        }

        double tSize = 2 * tan(DepthRenderer->GetActiveCamera()->GetViewAngle() * M_PI / 360.0);
        double sSize = tSize * AspectRatio;

        float cameraImagePlaneSize[2];
        cameraImagePlaneSize[0] = static_cast<float>(sSize);
        cameraImagePlaneSize[1] = static_cast<float>(tSize);

        ok = program->SetUniformf("cameraZNear", static_cast<float>(cameraZNear));
        if (!ok) {
            vtkErrorMacro(<< "Couldn't set cameraZNear");
        }

        ok = program->SetUniformf("cameraZFar", static_cast<float>(cameraZFar));
        if (!ok) {
            vtkErrorMacro(<< "Couldn't set cameraZFar");
        }

        ok = program->SetUniform3f("cameraOrientationWC", cameraOrientationWC);
        if (!ok) {
            vtkErrorMacro(<< "Couldn't set cameraOrientationWC");
        }

        ok = program->SetUniform3f("cameraPositionWC", cameraPositionWC);
        if (!ok) {
            vtkErrorMacro(<< "Couldn't set cameraPositionWC");
        }

        ok = program->SetUniform3f("cameraRightWC", cameraRightWC);
        if (!ok) {
            vtkErrorMacro(<< "Couldn't set cameraRightWC");
        }

        ok = program->SetUniform3f("cameraUpWC", cameraUpWC);
        if (!ok) {
            vtkErrorMacro(<< "Couldn't set cameraUpWC");
        }

        ok = program->SetUniform2f("cameraImagePlaneSize", cameraImagePlaneSize);
        if (!ok) {
            vtkErrorMacro(<< "Couldn't set cameraImagePlaneSize");
        }

        ok = program->SetUniformf("zTolerance", ZTolerance);
        if (!ok) {
            vtkErrorMacro(<< "Couldn't set zTolerance");
        }
    }

    assert(ProjectedTexture->GetTextureUnit() >= 0);
    ok = program->SetUniformi("cameraImage", ProjectedTexture->GetTextureUnit());
    if (!ok) {
        vtkErrorMacro(<< "Couldn't set cameraImage");
    }
    assert(DepthTexture->GetTextureUnit() >= 0);
    ok = program->SetUniformi("cameraDepth", DepthTexture->GetTextureUnit());
    if (!ok) {
        vtkErrorMacro(<< "Couldn't set cameraDepth");
    }
}
