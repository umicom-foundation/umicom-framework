/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/creative_asset_png_private.h
 *
 * PURPOSE:
 *   Share edited PNG preparation and export with the creative asset worker.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CREATIVE_ASSET_PNG_PRIVATE_H
#define UMICOM_CREATIVE_ASSET_PNG_PRIVATE_H

#include "umicom/media/image_edit.h"
#include "umicom/media/png_image.h"

UmiStatus UmiCreativePrepareEditedPng(const void *bytes, size_t size, const UmiMediaImageEdit *edit,
    const UmiCancellationToken *cancel, UmiMediaImageSurface **outImage);

UmiStatus UmiCreativeExportPreparedPng(const UmiMediaImageSurface *image, bool branded,
    const char *path, const UmiCancellationToken *cancel, UmiMediaPngWriteResult *out);

#endif
