/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/cpu_feature_set.h
 *
 * PURPOSE:
 *   Maintain CPU feature bitsets and compute missing-feature masks for runtime admission.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_CPU_FEATURE_SET_H
#define UMICOM_PLATFORM_CROSS_TARGET_CPU_FEATURE_SET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/cpu_feature.h"
/**
 * Represent the ct cpu feature set data shared with callers of this public contract.
 */
typedef struct UmiCtCpuFeatureSet { uint64_t bits; } UmiCtCpuFeatureSet;
/**
 * Add ct cpu feature set only after its inputs and available capacity have been checked.
 */
void umi_ct_cpu_feature_set_add(UmiCtCpuFeatureSet *set,UmiCtCpuFeature feature);
/**
 * Provide the ct cpu feature set has operation used by this module and its client
 * applications.
 */
bool umi_ct_cpu_feature_set_has(const UmiCtCpuFeatureSet *set,UmiCtCpuFeature feature);
/**
 * Provide the ct cpu feature set missing operation used by this module and its client
 * applications.
 */
uint64_t umi_ct_cpu_feature_set_missing(const UmiCtCpuFeatureSet *available,const UmiCtCpuFeatureSet *required);

#ifdef __cplusplus
}
#endif

#endif
