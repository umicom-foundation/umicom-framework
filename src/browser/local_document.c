/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/browser/local_document.c
 *
 * PURPOSE:
 *   Read bounded local HTML through one checked platform file handle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/browser/document_preview.h"
#include "umicom/platform/input_file.h"
#include <string.h>

UmiStatus umi_browser_document_preview_read_file(
    const char *absolute_path,
    size_t maximum_bytes,
    UmiBrowserDocumentPreview *out_preview)
{
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiStatus status;
    if (absolute_path == NULL || maximum_bytes == 0U || out_preview == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* A pathname size check followed by a second open cannot enforce a memory
     * budget. The platform reader owns both the handle and its bounded buffer. */
    status = UmiInputFileRead(absolute_path, maximum_bytes, &bytes, &size);
    if (status != UMI_STATUS_OK) return status;
    if (memchr(bytes, '\0', size) != NULL) {
        UmiInputFileFree(bytes);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Parsing accepts a non-null byte span and publishes only after admission.
     * All failures above leave the caller's previous preview intact. */
    status = umi_browser_document_preview_from_html((const char *)bytes, size, out_preview);
    UmiInputFileFree(bytes);
    return status;
}
