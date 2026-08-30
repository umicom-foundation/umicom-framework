/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/page_template.c
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
#include "umicom/designer/rad/page_template.h"
#include <string.h>
UmiStatus umi_rad_page_template_init(UmiRadPageTemplate *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->template_id, sizeof item->template_id, "page_template");
    (void)umi_rad_copy_text(item->name, sizeof item->name, "page_template");
    (void)umi_rad_copy_text(item->shell_kind, sizeof item->shell_kind, "page_template");
    return UMI_STATUS_OK;
}
int umi_rad_page_template_is_valid(const UmiRadPageTemplate *item){if(item==NULL)return 0;return umi_rad_id_valid(item->template_id) && item->name[0] != '\0';}
