/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media/png_file.c
 * PURPOSE: Compose PNG encoding and exclusive native file creation without changing the source image or replacing existing files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/media/png_image.h"
#include <string.h>
UmiStatus UmiMediaPngWriteNew(const UmiMediaImageSurface *surface, const char *path,
                              const UmiCancellationToken *cancel, UmiMediaPngWriteResult *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK)
        return status;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    status = UmiMediaPngEncode(surface, cancel, &bytes, &size);
    if (status != UMI_STATUS_OK)
        return status;
    UmiOutputFile *file = NULL;
    if (umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    else
        status = UmiOutputFileCreate(path, &file);
    if (status == UMI_STATUS_OK)
    {
        out->created = true;
        for (size_t offset = 0U; status == UMI_STATUS_OK && offset < size;)
        {
            if (umi_cancellation_token_is_requested(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            size_t count = size - offset;
            if (count > 65536U)
                count = 65536U;
            status = UmiOutputFileWrite(file, bytes + offset, count);
            offset += count;
        }
        if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
            status = UMI_STATUS_CANCELLED;
        UmiStatus closed = UmiOutputFileClose(file);
        if (status == UMI_STATUS_OK)
            status = closed;
        UmiStatus inspected = UmiOutputFileRead(file, &out->file);
        if (status == UMI_STATUS_OK)
            status = inspected;
    }
    UmiOutputFileDestroy(file);
    UmiMediaPngFree(bytes);
    return status;
}
