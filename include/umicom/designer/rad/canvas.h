/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/canvas.h
 *
 * PURPOSE:
 *   Describe a visual application design canvas and its revision state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_CANVAS_H
#define UMICOM_DESIGNER_RAD_CANVAS_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadCanvas {
    char document_id[UMI_RAD_ID_CAPACITY];
    char root_component_id[UMI_RAD_ID_CAPACITY];
    uint64_t revision;
    bool dirty;
} UmiRadCanvas;
UmiStatus umi_rad_canvas_init(UmiRadCanvas *item);
int umi_rad_canvas_is_valid(const UmiRadCanvas *item);
#ifdef __cplusplus
}
#endif
#endif
