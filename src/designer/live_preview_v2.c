/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/live_preview_v2.c
 *
 * PURPOSE:
 *   Refresh toolkit-neutral live previews and expose honest freshness state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/designer/live_preview_v2.h"

#include <string.h>

void umi_designer_live_preview_v2_init(UmiDesignerLivePreviewV2 *preview)
{
    if (preview == NULL) return;
    (void)memset(preview, 0, sizeof(*preview));
    umi_designer_preview_viewport_init(&preview->viewport);
    preview->health = UMI_DESIGNER_PREVIEW_IDLE;
}

void umi_designer_live_preview_v2_mark_stale(UmiDesignerLivePreviewV2 *preview)
{
    if (preview != NULL && preview->health == UMI_DESIGNER_PREVIEW_CURRENT) preview->health = UMI_DESIGNER_PREVIEW_STALE;
}

UmiStatus umi_designer_live_preview_v2_refresh(UmiDesignerLivePreviewV2 *preview,
                                               const UmiDesignerDocument *document,
                                               const UmiDeclSchema *schema)
{
    UmiDesignerDocumentSnapshot snapshot;
    UmiStatus status;
    if (preview == NULL || document == NULL || schema == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(&preview->diagnostics, 0, sizeof(preview->diagnostics));
    status = umi_designer_preview(document, schema, preview->rendered_text, sizeof(preview->rendered_text), &preview->diagnostics);
    if (status == UMI_STATUS_OK) {
        (void)umi_designer_document_snapshot(document, &snapshot);
        preview->source_revision = snapshot.revision;
        preview->refresh_count += 1U;
        preview->health = UMI_DESIGNER_PREVIEW_CURRENT;
    } else {
        preview->health = UMI_DESIGNER_PREVIEW_INVALID;
        preview->rendered_text[0] = '\0';
    }
    return status;
}

const char *umi_designer_live_preview_v2_text(const UmiDesignerLivePreviewV2 *preview)
{
    return preview != NULL ? preview->rendered_text : "";
}
