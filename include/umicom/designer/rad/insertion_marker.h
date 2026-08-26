/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/insertion_marker.h
 *
 * PURPOSE:
 *   Represent insertion feedback within ordered containers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_INSERTION_MARKER_H
#define UMICOM_DESIGNER_RAD_INSERTION_MARKER_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadInsertionMarker {
    char parent_id[UMI_RAD_ID_CAPACITY];
    int32_t index;
    UmiRadRect bounds;
    bool visible;
} UmiRadInsertionMarker;
UmiStatus umi_rad_insertion_marker_init(UmiRadInsertionMarker *item);
int umi_rad_insertion_marker_is_valid(const UmiRadInsertionMarker *item);
#ifdef __cplusplus
}
#endif
#endif
