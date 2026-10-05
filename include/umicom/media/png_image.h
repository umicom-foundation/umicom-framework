/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media/png_image.h
 * PURPOSE: Decode bounded PNG input into the shared owned RGBA surface for native, mobile and browser hosts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MEDIA_PNG_IMAGE_H
#define UMICOM_MEDIA_PNG_IMAGE_H
#include "umicom/media/image_surface.h"
#include "umicom/platform/cancellation.h"
#include "umicom/platform/output_file.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_MEDIA_PNG_INPUT_LIMIT (64U * 1024U * 1024U)
#define UMI_MEDIA_PNG_MAX_DIMENSION 8192U
#define UMI_MEDIA_PNG_MAX_PIXELS (16U * 1024U * 1024U)
    /* Available only when this Framework build includes the required libpng read
 * features. A missing optional decoder reports NOT_IMPLEMENTED, never an empty
 * successful image. No native window, file, process or network is required. */
    int UmiMediaPngAvailable(void);
    /* Initialise *out_surface=NULL; errors leave it unchanged and live outputs are
 * refused. Accept a complete static, noninterlaced PNG within the limits above.
 * Expand palette, grayscale and transparency to RGBA; reduce 16-bit samples to
 * their high eight bits. Pixels are straight alpha, in source sample values:
 * no ICC/gamma conversion, EXIF rotation or animation is applied.
 * The decoder checks framing and CRCs, caps libpng allocation, checks native
 * dimensions before pixel allocation, and cooperates with cancellation between
 * chunks/rows. A successful surface owns all pixels independently of input.
 * Run decoding on a worker and keep input bytes unchanged for the full call. */
    UmiStatus UmiMediaPngDecode(const void *bytes, size_t byte_count, const UmiCancellationToken *cancel,
                                UmiMediaImageSurface **out_surface);

    /* Encoding is independently optional. Emit complete static RGBA PNG with no
 * inherited metadata, palette or animation. Input sample values are preserved
 * without claiming a colour profile. Initialise *out_bytes=NULL. Output size
 * and pointer stay unchanged on failure; free successful bytes with PngFree.
 * Input dimensions/pixels and output byte size use the same bounds as decode.
 * Hold the input surface stable while a worker encodes it. */
    int UmiMediaPngEncoderAvailable(void);
    UmiStatus UmiMediaPngEncode(const UmiMediaImageSurface *surface, const UmiCancellationToken *cancel,
                                unsigned char **out_bytes, size_t *out_size);
    void UmiMediaPngFree(void *bytes);
    typedef struct UmiMediaPngWriteResult
    {
        bool created;
        UmiOutputFileSnapshot file;
    } UmiMediaPngWriteResult;
    /* Write a complete encoding to a new absolute file. Never overwrite, create
 * parents or delete partial output. A failed/cancelled write may leave a new
 * partial or complete file; the receipt records actual creation and byte count.
 * Native I/O may block; cancellation is checked between bounded writes. */
    UmiStatus UmiMediaPngWriteNew(const UmiMediaImageSurface *surface, const char *path,
                                  const UmiCancellationToken *cancel, UmiMediaPngWriteResult *out);
#ifdef __cplusplus
}
#endif
#endif
