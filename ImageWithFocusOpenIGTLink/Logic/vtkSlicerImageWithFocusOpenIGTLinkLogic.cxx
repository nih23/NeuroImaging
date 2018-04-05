#include "vtkSlicerImageWithFocusOpenIGTLinkLogic.h"

#include <cassert>

using namespace igtl;
using namespace std;

vtkStandardNewMacro(vtkSlicerImageWithFocusOpenIGTLinkLogic);

void
vtkSlicerImageWithFocusOpenIGTLinkLogic::SetOpenIGTLinkIFLogic(
    vtkSlicerOpenIGTLinkIFLogic *openIGTLinkIFLogic) {
    this->OpenIGTLinkIFLogic = openIGTLinkIFLogic;
}

vtkSlicerImageWithFocusOpenIGTLinkLogic::vtkSlicerImageWithFocusOpenIGTLinkLogic() {}

vtkSlicerImageWithFocusOpenIGTLinkLogic::~vtkSlicerImageWithFocusOpenIGTLinkLogic() {}

void
vtkSlicerImageWithFocusOpenIGTLinkLogic::RegisterNodes() {
    if (!this->GetMRMLScene()) {
        vtkErrorMacro("RegisterNodes: Invalid MRML scene");
        return;
    }

    if (this->OpenIGTLinkIFLogic) {
        OpenIGTLinkIFLogic->RegisterMessageConverter(this->IGTLToMRMLImageWithFocus);
    }
}
