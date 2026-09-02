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

/*
 * A platform matrix lets the release process distinguish Windows x64 packages from future Linux or RISC-V builds.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_PLATFORM_MATRIX_H
#define INCLUDE_UMICOM_DELIVERY_PLATFORM_MATRIX_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the platform target data shared with callers of this public contract.
 */
typedef struct UmiPlatformTarget {
    char operating_system[UMI_DELIVERY_ID_CAPACITY];
    char architecture[UMI_DELIVERY_ID_CAPACITY];
} UmiPlatformTarget;
/**
 * Represent the platform matrix data shared with callers of this public contract.
 */
typedef struct UmiPlatformMatrix {
    UmiPlatformTarget targets[32U];
    size_t count;
} UmiPlatformMatrix;
/**
 * Initialise platform matrix from caller-provided values so later operations receive a
 * known state.
 */
void umi_platform_matrix_init(UmiPlatformMatrix *matrix);
/**
 * Add platform matrix only after its inputs and available capacity have been checked.
 */
UmiStatus umi_platform_matrix_add(UmiPlatformMatrix *matrix,
                                  const char *operating_system,
                                  const char *architecture);
/**
 * Provide the platform matrix supports operation used by this module and its client
 * applications.
 */
int umi_platform_matrix_supports(const UmiPlatformMatrix *matrix,
                                 const char *operating_system,
                                 const char *architecture);

#ifdef __cplusplus
}
#endif

#endif
