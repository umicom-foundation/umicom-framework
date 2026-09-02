/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/insertion_marker.h
 *
 * PURPOSE:
 *   Represent insertion feedback within ordered containers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_INSERTION_MARKER_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_INSERTION_MARKER_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer insertion marker data shared with callers of this public contract.
 */
typedef struct UmiRadInsertionMarker {
    char parent_id[UMI_RAD_ID_CAPACITY];
    int32_t index;
    UmiRadRect bounds;
    bool visible;
} UmiRadInsertionMarker;
/**
 * Initialise visual designer insertion marker from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_insertion_marker_init(UmiRadInsertionMarker *item);
/**
 * Check that visual designer insertion marker satisfies its contract before another service relies on
 * it.
 */
int umi_rad_insertion_marker_is_valid(const UmiRadInsertionMarker *item);
#ifdef __cplusplus
}
#endif
#endif
