/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/event_descriptor.c
 *
 * PURPOSE:
 *   Describe an event exposed by a semantic component.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/event_descriptor.h"
#include <string.h>
UmiStatus umi_rad_event_descriptor_init(UmiRadEventDescriptor *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->event_id, sizeof item->event_id, "event_descriptor");
    (void)umi_rad_copy_text(item->label, sizeof item->label, "event_descriptor");
    (void)umi_rad_copy_text(item->parameter_type, sizeof item->parameter_type, "event_descriptor");
    item->bindable = true;
    return UMI_STATUS_OK;
}
int umi_rad_event_descriptor_is_valid(const UmiRadEventDescriptor *item){if(item==NULL)return 0;return umi_rad_id_valid(item->event_id);}
