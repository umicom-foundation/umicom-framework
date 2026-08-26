/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/drop_target.h
 *
 * PURPOSE:
 *   Represent validated parent/slot destinations during component drag and drop.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_DROP_TARGET_H
#define UMICOM_DESIGNER_RAD_DROP_TARGET_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadDropTarget {
    char parent_id[UMI_RAD_ID_CAPACITY];
    char slot_id[UMI_RAD_ID_CAPACITY];
    UmiRadRect bounds;
    bool accepted;
} UmiRadDropTarget;
UmiStatus umi_rad_drop_target_init(UmiRadDropTarget *item);
int umi_rad_drop_target_is_valid(const UmiRadDropTarget *item);
#ifdef __cplusplus
}
#endif
#endif
