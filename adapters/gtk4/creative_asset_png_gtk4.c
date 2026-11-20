/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/creative_asset_png_gtk4.c
 *
 * PURPOSE:
 *   Compose portable edits and canonical export branding without a GTK widget.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "creative_asset_png_private.h"
#include "umicom/ui/gtk4/image_brand.h"

UmiStatus UmiCreativePrepareEditedPng(const void *bytes, size_t size, const UmiMediaImageEdit *edit,
    const UmiCancellationToken *cancel, UmiMediaImageSurface **outImage)
{
    if (edit == NULL || outImage == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (*outImage != NULL) return UMI_STATUS_INVALID_STATE;

    UmiMediaImageSurface *image = NULL;
    UmiStatus status = UmiMediaPngDecode(bytes, size, cancel, &image);
    if (status != UMI_STATUS_OK) return status;

    UmiMediaImageSurfaceSnapshot info;
    status = umi_media_image_surface_snapshot(image, &info);
    UmiMediaImageEdit region = *edit;
    if (status == UMI_STATUS_OK && (region.x >= info.width || region.y >= info.height))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status != UMI_STATUS_OK) {
        umi_media_image_surface_destroy(image);
        return status;
    }
    /* Interpret the form's zero width and height once here. Preview and export
     * use the same edited coordinates and the same owned pixel surface. */
    if (region.width == 0U) region.width = info.width - region.x;
    if (region.height == 0U) region.height = info.height - region.y;
    if (region.x == 0U && region.y == 0U && region.width == info.width &&
        region.height == info.height && region.orientation == UMI_MEDIA_IMAGE_ORIGINAL) {
        *outImage = image;
        return UMI_STATUS_OK;
    }
    status = UmiMediaImageSurfaceApplyEdit(image, &region, cancel, outImage);
    umi_media_image_surface_destroy(image);
    return status;
}

UmiStatus UmiCreativeExportPreparedPng(const UmiMediaImageSurface *image, bool branded,
    const char *path, const UmiCancellationToken *cancel, UmiMediaPngWriteResult *out)
{
    /* Place the mark in final image coordinates so the crop cannot remove it
     * and a rotation cannot turn it sideways. Both writers own the new file. */
    if (branded) return UmiGtk4ImageWriteBrandedPng(image, path, cancel, out);
    return UmiMediaPngWriteNew(image, path, cancel, out);
}
