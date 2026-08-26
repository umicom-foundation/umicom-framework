/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/preview_target.h
 *
 * PURPOSE:
 *   Describe GTK4, Qt6, Native Web or abstract-device preview targets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_PREVIEW_TARGET_H
#define UMICOM_DESIGNER_RAD_PREVIEW_TARGET_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadPreviewTarget {
    char target_id[UMI_RAD_ID_CAPACITY];
    UmiRadTargetKind kind;
    UmiRadSize viewport;
    uint32_t dpi;
} UmiRadPreviewTarget;
UmiStatus umi_rad_preview_target_init(UmiRadPreviewTarget *item);
int umi_rad_preview_target_is_valid(const UmiRadPreviewTarget *item);
#ifdef __cplusplus
}
#endif
#endif
