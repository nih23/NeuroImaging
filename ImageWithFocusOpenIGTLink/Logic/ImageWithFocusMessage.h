#pragma once

#include <igtlImageMessage.h>

namespace igtl {
/**
 * @brief Custom image message that adds a focus value
 *
 * Initially developed by Florian Weidner
 */
class ImageWithFocusMessage : public ImageMessage {

public:
    typedef ImageWithFocusMessage Self;
    typedef ImageMessage Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    igtlTypeMacro(igtl::ImageWithFocusMessage, igtl::ImageMessage);
    igtlNewMacro(igtl::ImageWithFocusMessage);

    /**
     * @brief Sets the focus value.
     * @param f Focus value
     */
    void
    SetFocusValue(const int f) {
        m_FocusValue = f;
    }

    /**
     * @brief Gets the focus value.
     * @return
     */
    int
    GetFocusValue() {
        return m_FocusValue;
    }

    /**
     * @brief Creates a new ImageWithFocusMessage.
     */
    ImageWithFocusMessage();

    /**
     * @brief Destroys the ImageWithFocusMessage.
     */
    virtual ~ImageWithFocusMessage();

    /**
     * @brief Calculates the size of the message content (based on OpenIGTLinkv3)
     * @return Size in bytes
     */
    virtual int
    CalculateContentBufferSize() override;

    /**
     * @brief Packs the message content from the current state of the object.
     * @return
     */
    virtual int
    PackContent() override;

    /**
     * @brief Updates the object state from the currently read message.
     * @return
     */
    virtual int
    UnpackContent() override;

    /**
     * @brief Allocate the buffer for the image scalars.
     */
    void
    AllocateScalars();

protected:
    /**
     * @brief Discrete focus value
     */
    int m_FocusValue;
};
} // namespace igtl
