/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/action_binding.h
 *
 * PURPOSE:
 *   Bind a designer action surface to a Framework command and target.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_ACTION_BINDING_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_ACTION_BINDING_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer action binding data shared with callers of this public contract.
 */
typedef struct UmiRadActionBinding {
    char action_id[UMI_RAD_ID_CAPACITY];
    char command_id[UMI_RAD_ID_CAPACITY];
    char target_id[UMI_RAD_ID_CAPACITY];
    bool enabled;
} UmiRadActionBinding;
/**
 * Initialise visual designer action binding from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_action_binding_init(UmiRadActionBinding *item);
/**
 * Check that visual designer action binding satisfies its contract before another service relies on
 * it.
 */
int umi_rad_action_binding_is_valid(const UmiRadActionBinding *item);
#ifdef __cplusplus
}
#endif
#endif
