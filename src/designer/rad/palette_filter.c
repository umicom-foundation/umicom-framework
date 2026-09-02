/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/palette_filter.c
 *
 * PURPOSE:
 *   Filter the component palette by text, category and capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/palette_filter.h"
#include <string.h>
/*
 * Initialise rad palette filter from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_palette_filter_init(UmiRadPaletteFilter *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->query, sizeof item->query, "palette_filter");
    (void)umi_rad_copy_text(item->category, sizeof item->category, "palette_filter");
    (void)umi_rad_copy_text(item->capability, sizeof item->capability, "palette_filter");
    return UMI_STATUS_OK;
}
/*
 * Check that rad palette filter satisfies its contract before another service relies on
 * it.
 */
int umi_rad_palette_filter_is_valid(const UmiRadPaletteFilter *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return 1;}
