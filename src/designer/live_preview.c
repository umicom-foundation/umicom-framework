/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/live_preview.c
 *
 * PURPOSE:
 *   Refresh toolkit-neutral live previews and expose honest freshness state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/designer/live_preview.h"

#include <string.h>

/*
 * Initialise designer live preview from caller-provided values so later operations receive
 * a known state.
 */
void umi_designer_live_preview_init(UmiDesignerLivePreview *preview)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL) return;
    (void)memset(preview, 0, sizeof(*preview));
    umi_designer_preview_viewport_init(&preview->viewport);
    preview->health = UMI_DESIGNER_PREVIEW_IDLE;
}

/*
 * Provide the designer live preview mark stale operation used by this module and its
 * client applications.
 */
void umi_designer_live_preview_mark_stale(UmiDesignerLivePreview *preview)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview != NULL && preview->health == UMI_DESIGNER_PREVIEW_CURRENT) preview->health = UMI_DESIGNER_PREVIEW_STALE;
}

/*
 * Provide the designer live preview refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_live_preview_refresh(UmiDesignerLivePreview *preview,
                                               const UmiDesignerDocument *document,
                                               const UmiDeclSchema *schema)
{
    UmiDesignerDocumentSnapshot snapshot;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL || document == NULL || schema == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(&preview->diagnostics, 0, sizeof(preview->diagnostics));
    status = umi_designer_preview(document, schema, preview->rendered_text, sizeof(preview->rendered_text), &preview->diagnostics);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        (void)umi_designer_document_snapshot(document, &snapshot);
        preview->source_revision = snapshot.revision;
        preview->refresh_count += 1U;
        preview->health = UMI_DESIGNER_PREVIEW_CURRENT;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        preview->health = UMI_DESIGNER_PREVIEW_INVALID;
        preview->rendered_text[0] = '\0';
    }
    return status;
}

/*
 * Provide the designer live preview text operation used by this module and its client
 * applications.
 */
const char *umi_designer_live_preview_text(const UmiDesignerLivePreview *preview)
{
    return preview != NULL ? preview->rendered_text : "";
}
