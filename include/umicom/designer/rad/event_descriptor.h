/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/event_descriptor.h
 *
 * PURPOSE:
 *   Describe an event exposed by a semantic component.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_EVENT_DESCRIPTOR_H
#define UMICOM_DESIGNER_RAD_EVENT_DESCRIPTOR_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadEventDescriptor {
    char event_id[UMI_RAD_ID_CAPACITY];
    char label[UMI_RAD_TEXT_CAPACITY];
    char parameter_type[UMI_RAD_ID_CAPACITY];
    bool bindable;
} UmiRadEventDescriptor;
UmiStatus umi_rad_event_descriptor_init(UmiRadEventDescriptor *item);
int umi_rad_event_descriptor_is_valid(const UmiRadEventDescriptor *item);
#ifdef __cplusplus
}
#endif
#endif
