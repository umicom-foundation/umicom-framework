/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/action_binding.h
 *
 * PURPOSE:
 *   Bind a designer action surface to a Framework command and target.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_ACTION_BINDING_H
#define UMICOM_DESIGNER_RAD_ACTION_BINDING_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadActionBinding {
    char action_id[UMI_RAD_ID_CAPACITY];
    char command_id[UMI_RAD_ID_CAPACITY];
    char target_id[UMI_RAD_ID_CAPACITY];
    bool enabled;
} UmiRadActionBinding;
UmiStatus umi_rad_action_binding_init(UmiRadActionBinding *item);
int umi_rad_action_binding_is_valid(const UmiRadActionBinding *item);
#ifdef __cplusplus
}
#endif
#endif
