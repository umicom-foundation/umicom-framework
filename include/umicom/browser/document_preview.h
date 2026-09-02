/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/browser/document_preview.h
 *
 * PURPOSE:
 *   Convert an HTML document into a bounded, readable preview that native,
 *   terminal and accessibility frontends can display without owning an HTML
 *   rendering engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROWSER_DOCUMENT_PREVIEW_H
#define UMICOM_BROWSER_DOCUMENT_PREVIEW_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BROWSER_PREVIEW_TITLE_CAPACITY 256U
#define UMI_BROWSER_PREVIEW_TEXT_CAPACITY 65536U

/**
 * Hold the readable part of one HTML document. The fixed capacities make the
 * contract safe to use in a desktop panel without transferring ownership of
 * heap memory between Framework and application code.
 */
typedef struct UmiBrowserDocumentPreview {
    uint32_t structure_size;
    uint32_t api_version;
    char title[UMI_BROWSER_PREVIEW_TITLE_CAPACITY];
    char text[UMI_BROWSER_PREVIEW_TEXT_CAPACITY];
    size_t source_bytes;
    size_t text_bytes;
    int truncated;
} UmiBrowserDocumentPreview;

/** Initialise an empty preview before it is populated or reused. */
void umi_browser_document_preview_init(
    UmiBrowserDocumentPreview *preview);

/**
 * Extract visible words and the document title from bounded HTML input.
 * Script, style and markup content are not copied into the readable result.
 */
UmiStatus umi_browser_document_preview_from_html(
    const char *html,
    size_t html_size,
    UmiBrowserDocumentPreview *out_preview);

#ifdef __cplusplus
}
#endif
#endif
