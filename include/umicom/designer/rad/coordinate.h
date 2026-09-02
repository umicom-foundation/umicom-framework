/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/coordinate.h
 *
 * PURPOSE:
 *   Provide deterministic integer designer coordinates and translation operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_COORDINATE_H
#define UMICOM_DESIGNER_RAD_COORDINATE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad coordinate data shared with callers of this public contract.
 */
typedef struct UmiRadCoordinate {
    int32_t x;
    int32_t y;
} UmiRadCoordinate;
/**
 * Provide the rad coordinate make operation used by this module and its client
 * applications.
 */
UmiRadCoordinate umi_rad_coordinate_make(int32_t x, int32_t y);
/**
 * Provide the rad coordinate translate operation used by this module and its client
 * applications.
 */
UmiRadCoordinate umi_rad_coordinate_translate(UmiRadCoordinate point, int32_t dx, int32_t dy);
#ifdef __cplusplus
}
#endif
#endif
