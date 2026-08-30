/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/platform_matrix.h
 *
 * PURPOSE:
 *   Record target operating-system and architecture combinations supported by a release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A platform matrix lets the release process distinguish Windows x64 packages from future Linux or RISC-V builds.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_PLATFORM_MATRIX_H
#define INCLUDE_UMICOM_DELIVERY_PLATFORM_MATRIX_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPlatformTarget {
    char operating_system[UMI_DELIVERY_ID_CAPACITY];
    char architecture[UMI_DELIVERY_ID_CAPACITY];
} UmiPlatformTarget;
typedef struct UmiPlatformMatrix {
    UmiPlatformTarget targets[32U];
    size_t count;
} UmiPlatformMatrix;
void umi_platform_matrix_init(UmiPlatformMatrix *matrix);
UmiStatus umi_platform_matrix_add(UmiPlatformMatrix *matrix,
                                  const char *operating_system,
                                  const char *architecture);
int umi_platform_matrix_supports(const UmiPlatformMatrix *matrix,
                                 const char *operating_system,
                                 const char *architecture);

#ifdef __cplusplus
}
#endif

#endif
