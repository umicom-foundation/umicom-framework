/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/responsive_variant.c
 *
 * PURPOSE:
 *   Describe per-breakpoint component geometry and visibility overrides.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/responsive_variant.h"
#include <string.h>
UmiStatus umi_rad_responsive_variant_init(UmiRadResponsiveVariant *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->breakpoint_id, sizeof item->breakpoint_id, "responsive_variant");
    item->visible = true;
    return UMI_STATUS_OK;
}
int umi_rad_responsive_variant_is_valid(const UmiRadResponsiveVariant *item){if(item==NULL)return 0;return umi_rad_id_valid(item->breakpoint_id);}
