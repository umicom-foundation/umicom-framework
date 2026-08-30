/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/form_template.c
 *
 * PURPOSE:
 *   Describe reusable form templates and expected field/action counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/form_template.h"
#include <string.h>
UmiStatus umi_rad_form_template_init(UmiRadFormTemplate *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->template_id, sizeof item->template_id, "form_template");
    (void)umi_rad_copy_text(item->name, sizeof item->name, "form_template");
    item->field_count = 2U;
    item->action_count = 2U;
    return UMI_STATUS_OK;
}
int umi_rad_form_template_is_valid(const UmiRadFormTemplate *item){if(item==NULL)return 0;return umi_rad_id_valid(item->template_id) && item->name[0] != '\0';}
