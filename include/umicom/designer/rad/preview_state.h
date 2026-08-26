/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/preview_state.h
 *
 * PURPOSE:
 *   Record renderer-neutral preview health and diagnostic counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_PREVIEW_STATE_H
#define UMICOM_DESIGNER_RAD_PREVIEW_STATE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadPreviewState {
    uint64_t document_revision;
    uint64_t render_revision;
    size_t warning_count;
    size_t error_count;
    bool healthy;
} UmiRadPreviewState;
UmiStatus umi_rad_preview_state_init(UmiRadPreviewState *item);
int umi_rad_preview_state_is_valid(const UmiRadPreviewState *item);
#ifdef __cplusplus
}
#endif
#endif
