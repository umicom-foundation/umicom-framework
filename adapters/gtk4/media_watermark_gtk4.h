/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/media_watermark_gtk4.h
 *
 * PURPOSE:
 *   Own a private canonical SVG file for one native Media export.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MEDIA_WATERMARK_GTK4_H
#define UMICOM_MEDIA_WATERMARK_GTK4_H
#include "umicom/base/status.h"
typedef struct UmiMediaWatermarkFile UmiMediaWatermarkFile;
/* Worker-only resource I/O; no GTK widget or display access. Once allocation
 * succeeds, out owns a cleanup object even when Create reports an I/O failure.
 * Destroy releases it and reports failure to remove the exact temporary file. */
UmiStatus UmiMediaWatermarkFileCreate(UmiMediaWatermarkFile **out);
const char *UmiMediaWatermarkFilePath(const UmiMediaWatermarkFile *file);
UmiStatus UmiMediaWatermarkFileDestroy(UmiMediaWatermarkFile *file);
#endif
