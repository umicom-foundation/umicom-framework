/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/event_descriptor.h
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
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_EVENT_DESCRIPTOR_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_EVENT_DESCRIPTOR_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer event descriptor data shared with callers of this public contract.
 */
typedef struct UmiRadEventDescriptor {
    char event_id[UMI_RAD_ID_CAPACITY];
    char label[UMI_RAD_TEXT_CAPACITY];
    char parameter_type[UMI_RAD_ID_CAPACITY];
    bool bindable;
} UmiRadEventDescriptor;
/**
 * Initialise visual designer event descriptor from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_event_descriptor_init(UmiRadEventDescriptor *item);
/**
 * Check that visual designer event descriptor satisfies its contract before another service relies on
 * it.
 */
int umi_rad_event_descriptor_is_valid(const UmiRadEventDescriptor *item);
#ifdef __cplusplus
}
#endif
#endif
