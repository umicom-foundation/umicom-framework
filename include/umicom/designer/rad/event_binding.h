/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/event_binding.h
 *
 * PURPOSE:
 *   Bind a semantic component event to a Framework command identifier.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_EVENT_BINDING_H
#define UMICOM_DESIGNER_RAD_EVENT_BINDING_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadEventBinding {
    char binding_id[UMI_RAD_ID_CAPACITY];
    char component_id[UMI_RAD_ID_CAPACITY];
    char event_id[UMI_RAD_ID_CAPACITY];
    char command_id[UMI_RAD_ID_CAPACITY];
    bool enabled;
} UmiRadEventBinding;
UmiStatus umi_rad_event_binding_init(UmiRadEventBinding *item);
int umi_rad_event_binding_is_valid(const UmiRadEventBinding *item);
#ifdef __cplusplus
}
#endif
#endif
