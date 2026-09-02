/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/page_template.c
 *
 * PURPOSE:
 *   Describe reusable page templates without embedding application-specific logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/page_template.h"
#include <string.h>
/*
 * Initialise visual designer page template from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_page_template_init(UmiRadPageTemplate *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->template_id, sizeof item->template_id, "page_template");
    (void)umi_rad_copy_text(item->name, sizeof item->name, "page_template");
    (void)umi_rad_copy_text(item->shell_kind, sizeof item->shell_kind, "page_template");
    return UMI_STATUS_OK;
}
/* Check that visual designer page template satisfies its contract before another service relies on it. */
int umi_rad_page_template_is_valid(const UmiRadPageTemplate *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->template_id) && item->name[0] != '\0';}
