/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/form_descriptor.c
 *
 * PURPOSE:
 *   Describe a form, semantic root and Framework submit command.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/form_descriptor.h"
#include <string.h>
/*
 * Initialise visual designer form descriptor from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_form_descriptor_init(UmiRadFormDescriptor *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->form_id, sizeof item->form_id, "form_descriptor");
    (void)umi_rad_copy_text(item->title, sizeof item->title, "form_descriptor");
    (void)umi_rad_copy_text(item->root_component_id, sizeof item->root_component_id, "form_descriptor");
    (void)umi_rad_copy_text(item->submit_command_id, sizeof item->submit_command_id, "form_descriptor");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer form descriptor satisfies its contract before another service relies on
 * it.
 */
int umi_rad_form_descriptor_is_valid(const UmiRadFormDescriptor *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->form_id) && umi_rad_id_valid(item->root_component_id);}
