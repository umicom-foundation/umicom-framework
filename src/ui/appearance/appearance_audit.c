/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/appearance_audit.c
 *
 * PURPOSE:
 *   Aggregate appearance accessibility, scaling, typography and renderer-parity findings into one audit result.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_audit.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_audit_init(UmiAppearanceAppearanceAudit *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->audit_id,sizeof item->audit_id,"appearance.audit");
    item->checks=1U;
    item->passed=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_audit_is_valid(const UmiAppearanceAppearanceAudit *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->audit_id) && item->checks >= item->warnings + item->errors);
}
void umi_appearance_audit_evaluate(UmiAppearanceAppearanceAudit *item){if(item!=NULL)item->passed=item->errors==0U;}
