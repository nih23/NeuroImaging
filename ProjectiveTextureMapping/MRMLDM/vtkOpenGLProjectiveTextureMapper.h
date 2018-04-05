#pragma once

#include "vtkSlicerProjectiveTextureMappingModuleMRMLDisplayableManagerExport.h"

#include <vtkCamera.h>
#include <vtkEventForwarderCommand.h>
#include <vtkImageMapToColors.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkOpenGLTexture.h>

/**
 * @brief Projective texture mapper
 *
 * Implements the projective texture mapping visualisation
 */
class VTK_SLICER_PROJECTIVETEXTUREMAPPING_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT
    vtkOpenGLProjectiveTextureMapper : public vtkOpenGLPolyDataMapper {
public:
    /**
     * @brief Creates a new vtkOpenGLProjectiveTextureMapper.
     * @return New object
     */
    static vtkOpenGLProjectiveTextureMapper *
    New();

    vtkTypeMacro(vtkOpenGLProjectiveTextureMapper, vtkOpenGLPolyDataMapper);

    /**
     * @brief Prints the state of this object.
     * @param os Output stream
     * @param indent Identation that is prefixed before all outputs
     */
    void
    PrintSelf(ostream &os, vtkIndent indent) override;

    /**
     * @brief Starts the rendering process
     * @param ren Used Renderer
     * @param act Display actor
     */
    virtual void
    RenderPieceStart(vtkRenderer *ren, vtkActor *act) override;

    /**
     * @brief Finishes the rendering process
     * @param ren Used Renderer
     * @param act Display actor
     */
    virtual void
    RenderPieceFinish(vtkRenderer *ren, vtkActor *act) override;

    /**
     * Releases any graphics resources that are being consumed by this mapper.
     */
    virtual void
    ReleaseGraphicsResources(vtkWindow *) override;

    vtkSetMacro(ZTolerance, float);
    vtkGetMacro(ZTolerance, float);

    vtkSetMacro(AspectRatio, float);
    vtkGetMacro(AspectRatio, float);

    /**
     * @brief Sets the depth renderer is used to create the depth image of the surface model.
     * @param renderer Renderer
     */
    void
    SetDepthRenderer(vtkRenderer *renderer);
    vtkGetMacro(DepthRenderer, vtkRenderer *);

    /**
     * @brief Sets the input connection of the depth texture.
     * @param input Output connection
     */
    void
    SetDepthConnection(vtkAlgorithmOutput *input);

    /**
     * @brief Gets the input connection of the depth texture-
     * @return Output connection
     */
    vtkAlgorithmOutput *
    GetDepthConnection();

    /**
     * @brief Sets the input connection of the projected texture connection without a color map.
     * @param input Output connection
     */
    void
    SetProjectedTextureConnection(vtkAlgorithmOutput *input);

    /**
     * @brief Sets the input connection of the projecte texture connection with a color map.
     * @param input Output connection
     * @param colorMap Color map
     */
    void
    SetProjectedTextureConnection(vtkAlgorithmOutput *input, vtkScalarsToColors *colorMap);

    /**
     * @brief Gets the input connection of the projected texture.
     * @return Output connecion
     */
    vtkAlgorithmOutput *
    GetProjectedTextureConnection();

protected:
    /**
     * @brief Create new vtkOpenGLProjectiveTextureMapper.
     * @note Used by New()
     */
    vtkOpenGLProjectiveTextureMapper();

    /**
     * @brief Destroys the vtkOpenGLProjectiveTextureMapper.
     */
    virtual ~vtkOpenGLProjectiveTextureMapper() override;

    /**
     * @brief Replace the shader texture coordinate template
     * @param shaders Shaders that should be replaced
     * @param ren Used Renderer
     * @param act Displayed Actor
     */
    virtual void
    ReplaceShaderTCoord(std::map<vtkShader::Type, vtkShader *> shaders,
                        vtkRenderer *ren,
                        vtkActor *act) override;

    /**
     * @brief Replace the shader normal template
     * @param shaders Shaders that should be replaced
     * @param ren Used Renderer
     * @param act Displayed Actor
     */
    virtual void
    ReplaceShaderNormal(std::map<vtkShader::Type, vtkShader *> shaders,
                        vtkRenderer *ren,
                        vtkActor *act) override;

    /**
     * @brief Replace the shader vertex coordinate template
     * @param shaders Shaders that should be replaced
     * @param ren Used Renderer
     * @param act Displayed Actor
     */
    virtual void
    ReplaceShaderPositionVC(std::map<vtkShader::Type, vtkShader *> shaders,
                            vtkRenderer *ren,
                            vtkActor *act) override;

    /**
     * @brief Sets the uniforms for the camera template
     * @param cellBO OpenGL helper
     * @param ren Used renderer
     * @param act Displayed Actor
     */
    virtual void
    SetCameraShaderParameters(vtkOpenGLHelper &cellBO, vtkRenderer *ren, vtkActor *act) override;

    /**
     * @brief z tolerance as a fraction of the bounding box of the input poly data
     */
    float ZTolerance;

    /**
     * @brief Aspect ratio of the projected texture
     */
    float AspectRatio;

    /**
     * @brief Depth renderer that is used to create the depth image of the input polydata
     */
    vtkRenderer *DepthRenderer;

    /**
     * @brief Depth image of the input polydata
     */
    vtkNew<vtkOpenGLTexture> DepthTexture;

    /**
     * @brief Projected texture which is mapped through a color map
     * @warning This should be done in the shader, if a continuous interpolation is used
     */
    vtkNew<vtkImageMapToColors> MappedProjectedTexture;

    /**
     * @brief Projected texture
     */
    vtkNew<vtkOpenGLTexture> ProjectedTexture;

private:
    vtkOpenGLProjectiveTextureMapper(const vtkOpenGLProjectiveTextureMapper &) = delete;

    void
    operator=(const vtkOpenGLProjectiveTextureMapper &) = delete;
};
