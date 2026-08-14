/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/live_preview.h
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
#ifndef UMICOM_DESIGNER_LIVE_PREVIEW_H
#define UMICOM_DESIGNER_LIVE_PREVIEW_H

#include "umicom/designer/builder_types.h"
#include "umicom/designer/preview.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_PREVIEW_TEXT_CAPACITY 32768U

typedef struct UmiDesignerLivePreview {
    UmiDesignerPreviewViewport viewport;
    UmiDesignerPreviewHealth health;
    uint64_t source_revision;
    uint64_t refresh_count;
    UmiDeclDiagnosticList diagnostics;
    char rendered_text[UMI_DESIGNER_PREVIEW_TEXT_CAPACITY];
} UmiDesignerLivePreview;

void umi_designer_live_preview_init(UmiDesignerLivePreview *preview);
void umi_designer_live_preview_mark_stale(UmiDesignerLivePreview *preview);
UmiStatus umi_designer_live_preview_refresh(UmiDesignerLivePreview *preview,
                                               const UmiDesignerDocument *document,
                                               const UmiDeclSchema *schema);
const char *umi_designer_live_preview_text(const UmiDesignerLivePreview *preview);

#ifdef __cplusplus
}
#endif
#endif
