/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/live_preview.h
 *
 * PURPOSE:
 *   Track live-preview freshness, viewport, diagnostics and rendered output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Preview output is rebuilt from the semantic document. The health flag tells a
 * frontend whether it is current, stale or invalid without hiding diagnostics.
 */
#ifndef UMICOM_DESIGNER_LIVE_PREVIEW_H
#define UMICOM_DESIGNER_LIVE_PREVIEW_H

#include "umicom/designer/builder_types.h"
#include "umicom/designer/preview.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_PREVIEW_TEXT_CAPACITY 32768U

/**
 * Represent the designer live preview data shared with callers of this public contract.
 */
typedef struct UmiDesignerLivePreview {
    UmiDesignerPreviewViewport viewport;
    UmiDesignerPreviewHealth health;
    uint64_t source_revision;
    uint64_t refresh_count;
    UmiDeclDiagnosticList diagnostics;
    char rendered_text[UMI_DESIGNER_PREVIEW_TEXT_CAPACITY];
} UmiDesignerLivePreview;

/**
 * Initialise designer live preview from caller-provided values so later operations receive
 * a known state.
 */
void umi_designer_live_preview_init(UmiDesignerLivePreview *preview);
/**
 * Provide the designer live preview mark stale operation used by this module and its
 * client applications.
 */
void umi_designer_live_preview_mark_stale(UmiDesignerLivePreview *preview);
/**
 * Provide the designer live preview refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_live_preview_refresh(UmiDesignerLivePreview *preview,
                                               const UmiDesignerDocument *document,
                                               const UmiDeclSchema *schema);
/**
 * Provide the designer live preview text operation used by this module and its client
 * applications.
 */
const char *umi_designer_live_preview_text(const UmiDesignerLivePreview *preview);

#ifdef __cplusplus
}
#endif
#endif
