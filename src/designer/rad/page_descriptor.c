/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/page_descriptor.c
 *
 * PURPOSE:
 *   Describe a visual application page, route and semantic root component.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/page_descriptor.h"
#include <string.h>
/*
 * Initialise rad page descriptor from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_page_descriptor_init(UmiRadPageDescriptor *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->page_id, sizeof item->page_id, "page_descriptor");
    (void)umi_rad_copy_text(item->route, sizeof item->route, "page_descriptor");
    (void)umi_rad_copy_text(item->title, sizeof item->title, "page_descriptor");
    (void)umi_rad_copy_text(item->root_component_id, sizeof item->root_component_id, "page_descriptor");
    return UMI_STATUS_OK;
}
/*
 * Check that rad page descriptor satisfies its contract before another service relies on
 * it.
 */
int umi_rad_page_descriptor_is_valid(const UmiRadPageDescriptor *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->page_id) && item->route[0] != '\0' && umi_rad_id_valid(item->root_component_id);}
