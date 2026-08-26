/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/appearance_snapshot.c
 *
 * PURPOSE:
 *   Persist resolved appearance identity and revisions for deterministic session restore and visual tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_snapshot.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_snapshot_init(UmiAppearanceAppearanceSnapshot *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->snapshot_id,sizeof item->snapshot_id,"appearance.snapshot");
    (void)umi_appearance_copy_text(item->profile_id,sizeof item->profile_id,"appearance.default");
    (void)umi_appearance_copy_text(item->theme_pack_id,sizeof item->theme_pack_id,"theme.default.dark");
    item->effective_scale=1.0;
    item->semantic_revision=1U;
    item->fingerprint=1469598103934665603ULL;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_snapshot_is_valid(const UmiAppearanceAppearanceSnapshot *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->snapshot_id) && item->effective_scale > 0.0 && item->semantic_revision > 0U && item->fingerprint != 0U);
}
