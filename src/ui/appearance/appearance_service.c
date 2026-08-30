/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/appearance_service.c
 *
 * PURPOSE:
 *   Expose aggregate readiness for Framework-owned production appearance services consumed by every thin application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_service.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_service_init(UmiAppearanceAppearanceService *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->service_id,sizeof item->service_id,"ui.appearance");
    item->themes_ready=true;
    item->typography_ready=true;
    item->scaling_ready=true;
    item->accessibility_ready=true;
    item->renderers_ready=true;
    item->revision=1U;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_service_is_valid(const UmiAppearanceAppearanceService *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->service_id) && item->revision > 0U);
}
int umi_appearance_service_ready(const UmiAppearanceAppearanceService *item){return item!=NULL&&item->themes_ready&&item->typography_ready&&item->scaling_ready&&item->accessibility_ready&&item->renderers_ready;}
