#pragma once

#include "vtkSlicerImageFusionModuleMRMLDisplayableManagerExport.h"

#include <vtkImageAlgorithm.h>

/**
 * @brief Image fusion
 *
 * Implements weighted averaging and guided filtering.
 */
class VTK_SLICER_IMAGEFUSION_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT vtkImageFusion
    : public vtkImageAlgorithm {
public:
    /**
     * @brief Creates a new vtkImageFusion.
     * @return New object
     */
    static vtkImageFusion *
    New();

    vtkTypeMacro(vtkImageFusion, vtkImageAlgorithm);

    /**
     * @brief Possible image fusion methods
     */
    enum Method { WEIGHTED_AVERAGING, GUIDED_FILTERING };

    vtkSetMacro(Method, int);

    vtkGetMacro(Method, int);

    /**
     * @brief Sets the used image fusion method to weighted averaging.
     */
    void
    SetMethodToWeightedAveraging() {
        this->SetMethod(WEIGHTED_AVERAGING);
    }

    /**
     * @brief Sets the used image fusion method to guided filtering.
     */
    void
    SetMethodToGuidedFiltering() {
        this->SetMethod(GUIDED_FILTERING);
    }

    vtkSetClampMacro(FirstInputWeight, double, 0.0, 1.0);

    vtkGetMacro(FirstInputWeight, double);

protected:
    /**
     * @brief Creates a new vtkImageFusion.
     * @note Used by New()
     */
    vtkImageFusion();

    /**
     * @brief Destroys the vtkImageFusion.
     */
    virtual ~vtkImageFusion();

    /**
     * @brief Declares the available input ports for a given port
     * @param port Port
     * @param info Information object
     * @return Success status
     */
    int
    FillInputPortInformation(int port, vtkInformation *info) override;

    /**
     * @brief Requests data from the input ports. This implements the actual image fusion
     * algorithms.
     * @param request Request information object
     * @param inputVector Input information objects
     * @param outputVecotr Output information objects
     * @return Success status
     */
    int
    RequestData(vtkInformation *request,
                vtkInformationVector **inputVector,
                vtkInformationVector *outputVector) override;

private:
    vtkImageFusion(const vtkImageFusion &) = delete;

    void
    operator=(const vtkImageFusion &) = delete;

    /**
     * @brief Used image fusion method
     */
    int Method;

    /**
     * @brief Input weight of the first input for weighted averaging
     */
    double FirstInputWeight;
};
