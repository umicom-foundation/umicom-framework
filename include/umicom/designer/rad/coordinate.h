/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/coordinate.h
 *
 * PURPOSE:
 *   Provide deterministic integer designer coordinates and translation operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_COORDINATE_H
#define UMICOM_DESIGNER_RAD_COORDINATE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadCoordinate {
    int32_t x;
    int32_t y;
} UmiRadCoordinate;
UmiRadCoordinate umi_rad_coordinate_make(int32_t x, int32_t y);
UmiRadCoordinate umi_rad_coordinate_translate(UmiRadCoordinate point, int32_t dx, int32_t dy);
#ifdef __cplusplus
}
#endif
#endif
