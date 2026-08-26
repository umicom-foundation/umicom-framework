/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/resize_handle.h
 *
 * PURPOSE:
 *   Describe resize-handle semantics without depending on a toolkit cursor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_RESIZE_HANDLE_H
#define UMICOM_DESIGNER_RAD_RESIZE_HANDLE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadResizeHandle {
    uint32_t edges;
    UmiRadPoint location;
    bool enabled;
} UmiRadResizeHandle;
UmiStatus umi_rad_resize_handle_init(UmiRadResizeHandle *item);
int umi_rad_resize_handle_is_valid(const UmiRadResizeHandle *item);
#ifdef __cplusplus
}
#endif
#endif
