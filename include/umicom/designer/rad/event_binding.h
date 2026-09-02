/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/event_binding.h
 *
 * PURPOSE:
 *   Bind a semantic component event to a Framework command identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_EVENT_BINDING_H
#define UMICOM_DESIGNER_RAD_EVENT_BINDING_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad event binding data shared with callers of this public contract.
 */
typedef struct UmiRadEventBinding {
    char binding_id[UMI_RAD_ID_CAPACITY];
    char component_id[UMI_RAD_ID_CAPACITY];
    char event_id[UMI_RAD_ID_CAPACITY];
    char command_id[UMI_RAD_ID_CAPACITY];
    bool enabled;
} UmiRadEventBinding;
/**
 * Initialise rad event binding from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_event_binding_init(UmiRadEventBinding *item);
/**
 * Check that rad event binding satisfies its contract before another service relies on it.
 */
int umi_rad_event_binding_is_valid(const UmiRadEventBinding *item);
#ifdef __cplusplus
}
#endif
#endif
