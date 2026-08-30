/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/focus_evidence.c
 *
 * PURPOSE:
 *   Record keyboard reachability and visible-focus evidence for a semantic interactive element.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/focus_evidence.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_focus_evidence_init(UmiAppearanceFocusEvidence *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->element_id,sizeof item->element_id,"button.submit");
    item->keyboard_reachable=true;
    item->visible_indicator=true;
    item->order_defined=true;
    item->passed=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_focus_evidence_is_valid(const UmiAppearanceFocusEvidence *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->element_id) && item->passed == (item->keyboard_reachable && item->visible_indicator && item->order_defined));
}
