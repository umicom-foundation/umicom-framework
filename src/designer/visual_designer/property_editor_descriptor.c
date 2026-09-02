/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/property_editor_descriptor.c
 *
 * PURPOSE:
 *   Describe an editor choice for a semantic component property.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/property_editor_descriptor.h"
#include <string.h>
/*
 * Initialise visual designer property editor descriptor from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_rad_property_editor_descriptor_init(UmiRadPropertyEditorDescriptor *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->property_id, sizeof item->property_id, "property_editor_descriptor");
    (void)umi_rad_copy_text(item->editor_type, sizeof item->editor_type, "property_editor_descriptor");
    (void)umi_rad_copy_text(item->value_type, sizeof item->value_type, "property_editor_descriptor");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer property editor descriptor satisfies its contract before another service
 * relies on it.
 */
int umi_rad_property_editor_descriptor_is_valid(const UmiRadPropertyEditorDescriptor *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->property_id) && umi_rad_id_valid(item->editor_type);}
