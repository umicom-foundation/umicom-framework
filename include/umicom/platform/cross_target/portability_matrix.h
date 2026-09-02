/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/portability_matrix.h
 *
 * PURPOSE:
 *   Record support levels for platform semantics across target operating systems.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_PORTABILITY_MATRIX_H
#define UMICOM_PLATFORM_CROSS_TARGET_PORTABILITY_MATRIX_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/portability_contract.h"
/**
 * Represent the ct portability entry data shared with callers of this public contract.
 */
typedef struct UmiCtPortabilityEntry { UmiCtOperatingSystem os; UmiCtPortabilityCapability capability; UmiCtSupportLevel support; } UmiCtPortabilityEntry;
/**
 * Represent the ct portability matrix data shared with callers of this public contract.
 */
typedef struct UmiCtPortabilityMatrix { UmiCtPortabilityEntry items[UMI_CT_MAX_ITEMS]; size_t count; } UmiCtPortabilityMatrix;
/**
 * Initialise ct portability matrix from caller-provided values so later operations receive
 * a known state.
 */
void umi_ct_portability_matrix_init(UmiCtPortabilityMatrix *matrix);
/**
 * Copy ct portability matrix into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_ct_portability_matrix_set(UmiCtPortabilityMatrix *matrix,UmiCtOperatingSystem os,UmiCtPortabilityCapability capability,UmiCtSupportLevel support);
/**
 * Provide the ct portability matrix get operation used by this module and its client
 * applications.
 */
UmiCtSupportLevel umi_ct_portability_matrix_get(const UmiCtPortabilityMatrix *matrix,UmiCtOperatingSystem os,UmiCtPortabilityCapability capability);

#ifdef __cplusplus
}
#endif

#endif
