/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/platform_conformance.h
 *
 * PURPOSE:
 *   Score target support across required portability, CPU and OS service capabilities.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_PLATFORM_CONFORMANCE_H
#define UMICOM_PLATFORM_CROSS_TARGET_PLATFORM_CONFORMANCE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/portability_contract.h"
typedef struct UmiCtPlatformConformanceInput { uint64_t portability_required; uint64_t portability_available; uint64_t cpu_required; uint64_t cpu_available; uint32_t required_services; uint32_t ready_services; } UmiCtPlatformConformanceInput;
typedef struct UmiCtPlatformConformance { uint8_t score; uint32_t blockers; uint32_t portability_gaps; uint32_t cpu_gaps; uint32_t service_gaps; UmiCtHealth health; } UmiCtPlatformConformance;
UmiCtPlatformConformance umi_ct_platform_conformance_evaluate(const UmiCtPlatformConformanceInput *input);

#ifdef __cplusplus
}
#endif

#endif
