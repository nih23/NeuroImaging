
#include "vtkIGTLToMRMLImageWithFocus.h"
#include "vtkMRMLIGTLQueryNode.h"

#include <igtlImageMessage.h>
#include <igtl_util.h>

#include <vtkMRMLColorLogic.h>
#include <vtkMRMLColorTableNode.h>

#include <vtkMRMLScalarVolumeDisplayNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLVectorVolumeDisplayNode.h>
#include <vtkMRMLVectorVolumeNode.h>

#include <vtkImageData.h>
#include <vtkIntArray.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

#include <vtksys/SystemTools.hxx>

#include "vtkSlicerOpenIGTLinkIFLogic.h"

vtkStandardNewMacro(vtkIGTLToMRMLImageWithFocus);

vtkIGTLToMRMLImageWithFocus::vtkIGTLToMRMLImageWithFocus() {}

vtkIGTLToMRMLImageWithFocus::~vtkIGTLToMRMLImageWithFocus() {}

vtkMRMLNode *
vtkIGTLToMRMLImageWithFocus::CreateNewNodeWithMessage(
    vtkMRMLScene *scene, const char *name, igtl::MessageBase::Pointer incomingImageMessage) {
    igtl::MessageBase *innerPtr = incomingImageMessage.GetPointer();
    if (innerPtr == NULL) {
        vtkErrorMacro(
            "Unable to create MRML node from incoming MYIMAGE message: incomingImageMessage is invalid");
        return 0;
    }

    igtl::ImageWithFocusMessage::Pointer imgMsg = igtl::ImageWithFocusMessage::New();
    // As this method is only called when the first image message is received,
    // (it is called when receiving each image message) the performance impact of the
    // additional Copy() and Unpack() method calls are negligible.
    imgMsg->Copy(incomingImageMessage.GetPointer());
    // Deserialize the transform data
    // If CheckCRC==0, CRC check is skipped.
    int c = imgMsg->Unpack(this->CheckCRC);
    if ((c & igtl::MessageHeader::UNPACK_BODY) == 0) // if CRC check fails
    {
        vtkErrorMacro(
            "Unable to create MRML node from incoming MYIMAGE message. Failed to unpack the message");
        return 0;
    }

    vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
    int numberOfComponents = imgMsg->GetNumComponents();
    // float fov = 256.0;
    image->SetDimensions(256, 256, 1);
    image->SetExtent(0, 255, 0, 255, 0, 0);
    // image->SetSpacing( fov/256, fov/256, 10 );
    image->SetSpacing(1.0, 1.0, 1.0);
    // image->SetOrigin( fov/2, -fov/2, -0.0 );
    image->SetOrigin(0.0, 0.0, 0.0);
#if (VTK_MAJOR_VERSION <= 5)
    image->SetNumberOfScalarComponents(numberOfComponents);
    image->SetScalarTypeToShort();
    image->AllocateScalars();
#else
    image->AllocateScalars(VTK_SHORT, numberOfComponents);
#endif

    short *dest = (short *) image->GetScalarPointer();
    if (dest) {
        memset(dest, 0x00, 256 * 256 * sizeof(short));
#if (VTK_MAJOR_VERSION <= 5)
        image->Update();
#endif
    }
    vtkSmartPointer<vtkMRMLVolumeNode> volumeNode;
    if (numberOfComponents == 1) {
        volumeNode = vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();
    } else if (numberOfComponents > 1) {
        volumeNode = vtkSmartPointer<vtkMRMLVectorVolumeNode>::New();
    }
    volumeNode->SetAndObserveImageData(image);
    volumeNode->SetName(name);

    scene->SaveStateForUndo();

    vtkDebugMacro("Setting scene info");
    volumeNode->SetScene(scene);
    volumeNode->SetDescription("Received by OpenIGTLink");

    /// double range[2];
    vtkDebugMacro("Set basic display info");
    // volumeNode->GetImageData()->GetScalarRange(range);
    // range[0] = 0.0;
    // range[1] = 256.0;
    // displayNode->SetLowerThreshold(range[0]);
    // displayNode->SetUpperThreshold(range[1]);
    // displayNode->SetWindow(range[1] - range[0]);
    // displayNode->SetLevel(0.5 * (range[1] + range[0]) );

    vtkDebugMacro("Name vol node " << volumeNode->GetClassName());
    vtkMRMLNode *n = scene->AddNode(volumeNode);

    this->SetDefaultDisplayNode(volumeNode, numberOfComponents);

    vtkDebugMacro("Node added to scene");

    this->CenterImage(volumeNode);

    return n;
}

int
vtkIGTLToMRMLImageWithFocus::UnpackIGTLMessage(igtl::MessageBase::Pointer buffer) {
    if (this->InImageWithFocusMessage.IsNull()) {
        this->InImageWithFocusMessage = igtl::ImageWithFocusMessage::New();
    }
    this->InImageWithFocusMessage->Copy(buffer);

    // Deserialize the transform data
    // If CheckCRC==0, CRC check is skipped.
    int c = this->InImageWithFocusMessage->Unpack(this->CheckCRC);

    if (this->InImageMessage.IsNull()) {
        this->InImageMessage = igtl::ImageMessage::New();
    }

    if ((c & igtl::MessageHeader::UNPACK_BODY) == 0) {
        return 0;
    }

    int coordinateSystem, dimensions[3], endian, numComponents, scalarType, subDimensions[3],
        offsets[3];
    igtl::Matrix4x4 mat;
    float spacing[3];
    std::string deviceName;
    unsigned timeStampSec, timeStampFrac;
    coordinateSystem = this->InImageWithFocusMessage->GetCoordinateSystem();
    this->InImageWithFocusMessage->GetDimensions(dimensions);
    endian = this->InImageWithFocusMessage->GetEndian();
    this->InImageWithFocusMessage->GetMatrix(mat);
    numComponents = this->InImageWithFocusMessage->GetNumComponents();
    scalarType = this->InImageWithFocusMessage->GetScalarType();
    this->InImageWithFocusMessage->GetSpacing(spacing);
    this->InImageWithFocusMessage->GetSubVolume(subDimensions, offsets);
    deviceName = this->InImageWithFocusMessage->GetDeviceName();
    this->InImageWithFocusMessage->GetTimeStamp(&timeStampSec, &timeStampFrac);

    this->InImageMessage->SetCoordinateSystem(coordinateSystem);
    this->InImageMessage->SetDimensions(dimensions);
    this->InImageMessage->SetEndian(endian);
    this->InImageMessage->SetMatrix(mat);
    this->InImageMessage->SetNumComponents(numComponents);
    this->InImageMessage->SetScalarType(scalarType);
    this->InImageMessage->SetSpacing(spacing);
    this->InImageMessage->SetSubVolume(subDimensions, offsets);
    this->InImageMessage->SetDeviceName(deviceName.c_str());
    this->InImageMessage->SetTimeStamp(timeStampSec, timeStampFrac);
    this->InImageMessage->AllocateScalars();
    memcpy(this->InImageMessage->GetScalarPointer(),
           this->InImageWithFocusMessage->GetScalarPointer(),
           this->InImageWithFocusMessage->GetSubVolumeImageSize());
    return 1;
}
