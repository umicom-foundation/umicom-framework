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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiCtPortabilityEntry { UmiCtOperatingSystem os; UmiCtPortabilityCapability capability; UmiCtSupportLevel support; } UmiCtPortabilityEntry;
typedef struct UmiCtPortabilityMatrix { UmiCtPortabilityEntry items[UMI_CT_MAX_ITEMS]; size_t count; } UmiCtPortabilityMatrix;
void umi_ct_portability_matrix_init(UmiCtPortabilityMatrix *matrix);
UmiStatus umi_ct_portability_matrix_set(UmiCtPortabilityMatrix *matrix,UmiCtOperatingSystem os,UmiCtPortabilityCapability capability,UmiCtSupportLevel support);
UmiCtSupportLevel umi_ct_portability_matrix_get(const UmiCtPortabilityMatrix *matrix,UmiCtOperatingSystem os,UmiCtPortabilityCapability capability);

#ifdef __cplusplus
}
#endif

#endif
