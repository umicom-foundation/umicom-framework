/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/cross_target_health.h
 *
 * PURPOSE:
 *   Aggregate cross-toolchain, sysroot, emulator and runtime conformance evidence into one readiness state.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_CROSS_TARGET_HEALTH_H
#define UMICOM_PLATFORM_CROSS_TARGET_CROSS_TARGET_HEALTH_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCtCrossTargetHealthInput { bool toolchain_required; bool toolchain_ready; bool sysroot_required; bool sysroot_ready; bool emulator_required; bool emulator_ready; bool runtime_conformant; uint32_t warnings; } UmiCtCrossTargetHealthInput;
typedef struct UmiCtCrossTargetHealth { UmiCtHealth health; uint32_t blockers; uint32_t warnings; uint8_t readiness_percent; } UmiCtCrossTargetHealth;
UmiCtCrossTargetHealth umi_ct_cross_target_health_evaluate(const UmiCtCrossTargetHealthInput *input);

#ifdef __cplusplus
}
#endif

#endif
