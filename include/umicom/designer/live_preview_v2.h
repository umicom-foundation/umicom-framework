/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/live_preview_v2.h
 *
 * PURPOSE:
 *   Track live-preview freshness, viewport, diagnostics and rendered output.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Preview output is rebuilt from the semantic document. The health flag tells a
 * frontend whether it is current, stale or invalid without hiding diagnostics.
 */
#ifndef UMICOM_DESIGNER_LIVE_PREVIEW_V2_H
#define UMICOM_DESIGNER_LIVE_PREVIEW_V2_H

#include "umicom/designer/builder_types_v2.h"
#include "umicom/designer/preview.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_V2_PREVIEW_TEXT_CAPACITY 32768U

typedef struct UmiDesignerLivePreviewV2 {
    UmiDesignerPreviewViewport viewport;
    UmiDesignerPreviewHealthV2 health;
    uint64_t source_revision;
    uint64_t refresh_count;
    UmiDeclDiagnosticList diagnostics;
    char rendered_text[UMI_DESIGNER_V2_PREVIEW_TEXT_CAPACITY];
} UmiDesignerLivePreviewV2;

void umi_designer_live_preview_v2_init(UmiDesignerLivePreviewV2 *preview);
void umi_designer_live_preview_v2_mark_stale(UmiDesignerLivePreviewV2 *preview);
UmiStatus umi_designer_live_preview_v2_refresh(UmiDesignerLivePreviewV2 *preview,
                                               const UmiDesignerDocument *document,
                                               const UmiDeclSchema *schema);
const char *umi_designer_live_preview_v2_text(const UmiDesignerLivePreviewV2 *preview);

#ifdef __cplusplus
}
#endif
#endif
