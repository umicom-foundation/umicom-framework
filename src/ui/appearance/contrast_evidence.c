/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/contrast_evidence.c
 *
 * PURPOSE:
 *   Persist auditable foreground/background token and ratio evidence for conformance reports.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/contrast_evidence.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_contrast_evidence_init(UmiAppearanceContrastEvidence *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->evidence_id,sizeof item->evidence_id,"contrast.text.primary");
    (void)umi_appearance_copy_text(item->foreground_token,sizeof item->foreground_token,"text.primary");
    (void)umi_appearance_copy_text(item->background_token,sizeof item->background_token,"surface.background");
    item->ratio=7.0;
    item->passed=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_contrast_evidence_is_valid(const UmiAppearanceContrastEvidence *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->evidence_id) && item->ratio >= 1.0);
}
