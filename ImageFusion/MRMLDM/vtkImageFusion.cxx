#include "vtkImageFusion.h"

#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <vector>

using namespace cv;
using namespace std;

vtkStandardNewMacro(vtkImageFusion);

int
vtkImageFusion::FillInputPortInformation(int port, vtkInformation *info) {
    if (port == 0) {
        info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
        info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
        return 1;
    }

    vtkErrorMacro("This filter does not have more than 1 input port!");
    return 0;
}

vtkImageFusion::vtkImageFusion() : Method{WEIGHTED_AVERAGING}, FirstInputWeight{0.5} {}
vtkImageFusion::~vtkImageFusion() {}

#define showImage(mat)                                \
    {                                                 \
        Mat windowImg;                                \
        if ((mat).channels() == 4) {                  \
            cvtColor((mat), windowImg, CV_RGBA2BGRA); \
        } else if ((mat).channels() == 3) {           \
            cvtColor((mat), windowImg, CV_RGB2BGR);   \
        } else {                                      \
            windowImg = (mat).clone();                \
        }                                             \
        flip(windowImg, windowImg, 0);                \
        imshow(#mat, windowImg);                      \
        while (waitKey() != ' ')                      \
            ;                                         \
        destroyWindow(#mat);                          \
    }

void
AOverB(Mat const &imgA, Mat const &imgB, Scalar backgroundColor, Mat &result) {
    Mat channelsA[4];
    Mat channelsB[4];
    Mat channelsRes[4];

    split(imgA, channelsA);
    split(imgB, channelsB);

    channelsRes[3] = channelsA[3] + (1 - channelsA[3]).mul(channelsB[3]);
    for (int i = 0; i < 3; ++i) {
        channelsRes[i] = (channelsA[3].mul(channelsA[i]) +
                          (1 - channelsA[3]).mul(channelsB[3]).mul(channelsB[i])) /
            channelsRes[3];
        channelsRes[i] =
            channelsRes[3].mul(channelsRes[i]) + (1 - channelsRes[3]) * backgroundColor[i];
    }

    vector<Mat> rgbChannelsRes{channelsRes[0], channelsRes[1], channelsRes[2]};

    merge(rgbChannelsRes, result);
}

void
removeColorInformation(Mat &img) {
    Mat channels[4];
    split(img, channels);
    merge(channels, 3, img);
    cvtColor(img, img, CV_RGB2GRAY);
    cvtColor(img, img, CV_GRAY2BGR);
    split(img, channels);
    merge(channels, 4, img);
}

void
guidedFilter(Mat const &I, Mat const &P, int r, double eps, Mat &filteredP) {
    Size windowSize{2 * r + 1, 2 * r + 1};
    Mat meanI, meanII;
    boxFilter(I, meanI, -1, windowSize);
    boxFilter(I.mul(I), meanII, -1, windowSize);
    Mat varianceI = meanII - meanI.mul(meanI);

    Mat meanP, meanIP;
    boxFilter(P, meanP, -1, windowSize);
    boxFilter(I.mul(P), meanIP, -1, windowSize);
    Mat covarianceIP = meanIP - meanI.mul(meanP);

    Mat a = covarianceIP / (varianceI + eps);
    Mat b = meanP - a.mul(meanI);

    Mat meanA, meanB;
    boxFilter(a, meanA, -1, windowSize);
    boxFilter(b, meanB, -1, windowSize);

    filteredP = meanA.mul(I) + meanB;
}

void
fuseWithGuidedFiltering(Mat img[2], Mat &fusedImage) {
    Mat base[2], detail[2], saliency[2];
    for (int i = 0; i < 2; ++i) {
        boxFilter(img[i], base[i], -1, Size(31, 31));

        detail[i] = img[i] - base[i];

        Laplacian(img[i], saliency[i], img[i].depth());
        absdiff(saliency[i], Scalar::all(0), saliency[i]);
        GaussianBlur(saliency[i], saliency[i], Size(11, 11), 5);
    }

    Mat winner[2], weightMapDetail[2], weightMapBase[2];
    for (int i = 0; i < 2; ++i) {
        winner[i] = saliency[i] == max(saliency[0], saliency[1]);
        winner[i].convertTo(winner[i], img[i].depth(), 1 / 255.0);
        guidedFilter(img[i], winner[i], 45, 0.3, weightMapBase[i]);
        guidedFilter(img[i], winner[i], 7, 1.0e-6, weightMapDetail[i]);
    }
    Mat summedWeightMapBase = weightMapBase[0] + weightMapBase[1];
    Mat summedWeightMapDetail = weightMapDetail[0] + weightMapDetail[1];
    for (int i = 0; i < 2; ++i) {
        weightMapBase[i] = weightMapBase[i] / summedWeightMapBase;
        weightMapDetail[i] = weightMapDetail[i] / summedWeightMapDetail;
    }

    Mat fusedBase = weightMapBase[0].mul(base[0]) + weightMapBase[1].mul(base[1]);
    Mat fusedDetail = weightMapDetail[0].mul(detail[0]) + weightMapDetail[1].mul(detail[1]);
    fusedImage = fusedBase + fusedDetail;
}

int
vtkImageFusion::RequestData(vtkInformation *vtkNotUsed(request),
                            vtkInformationVector **inputVector,
                            vtkInformationVector *outputVector) {
    // Get the info objects
    if (inputVector[0]->GetNumberOfInformationObjects() != 2) {
        vtkErrorMacro("This filter requires exactly 2 vtkImageData inputs at port 0");
        return 0;
    }
    vtkInformation *infoIn1 = inputVector[0]->GetInformationObject(0);
    vtkInformation *infoIn2 = inputVector[0]->GetInformationObject(1);
    vtkInformation *infoOut = outputVector->GetInformationObject(0);

    vtkImageData *imageIn1 = vtkImageData::SafeDownCast(infoIn1->Get(vtkDataObject::DATA_OBJECT()));
    vtkImageData *imageIn2 = vtkImageData::SafeDownCast(infoIn2->Get(vtkDataObject::DATA_OBJECT()));
    vtkImageData *imageOut = vtkImageData::SafeDownCast(infoOut->Get(vtkDataObject::DATA_OBJECT()));

    int extentIn1[6], extentIn2[6];
    imageIn1->GetExtent(extentIn1);
    imageIn2->GetExtent(extentIn2);
    for (int i = 0; i < 6; ++i) {
        if (extentIn1[i] != extentIn2[i]) {
            vtkErrorMacro("Input images have different extent");
            return 0;
        }
    }

    int numberOfComponents1 = imageIn1->GetNumberOfScalarComponents();
    int numberOfComponents2 = imageIn2->GetNumberOfScalarComponents();
    if (numberOfComponents1 != 4 || numberOfComponents2 != 4) {
        vtkErrorMacro("Input images should be RGB (3 channels)");
        return 0;
    }

    int dimensions[3];
    imageIn1->GetDimensions(dimensions);
    imageOut->CopyStructure(imageIn1);
    imageOut->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

    Mat out{dimensions[1], dimensions[0], CV_8UC3, imageOut->GetScalarPointer()};
    Mat in1{dimensions[1], dimensions[0], CV_8UC4, imageIn1->GetScalarPointer()};
    Mat in2{dimensions[1], dimensions[0], CV_8UC4, imageIn2->GetScalarPointer()};

    Mat img1, img2;
    in1.convertTo(img1, CV_32F, 1.0 / 255.0);
    in2.convertTo(img2, CV_32F, 1.0 / 255.0);

    removeColorInformation(img1);

    Mat img1Over2, img2Over1;
    Scalar backgroundColor{0.3, 0.3, 0.3};
    AOverB(img1, img2, backgroundColor, img1Over2);
    AOverB(img2, img1, backgroundColor, img2Over1);

    cvtColor(img1Over2, img1Over2, CV_RGB2HSV);
    cvtColor(img2Over1, img2Over1, CV_RGB2HSV);

    Mat channels1Over2[3], channels2Over1[3];
    split(img1Over2, channels1Over2);
    split(img2Over1, channels2Over1);

    Mat lumas[2] = {channels1Over2[2], channels2Over1[2]};
    Mat fusedLuma;
    if (this->Method == GUIDED_FILTERING) {
        fuseWithGuidedFiltering(lumas, fusedLuma);
    } else {
        fusedLuma = this->FirstInputWeight * lumas[0] + (1 - this->FirstInputWeight) * lumas[1];
    }

    vector<Mat> fusedChannels{channels2Over1[0], channels2Over1[1], fusedLuma};
    Mat result, resultWithAveraging;
    merge(fusedChannels, result);

    cvtColor(result, result, CV_HSV2RGB);
    result.convertTo(result, CV_8U, 255.0);
    result.copyTo(out);

    infoOut->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), extentIn1, 6);
    infoOut->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extentIn1, 6);

    return 1;
}
