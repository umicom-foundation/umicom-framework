/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/cross_target_snapshot.h
 *
 * PURPOSE:
 *   Aggregate immutable cross-target platform evidence for diagnostics, release gates and remote execution.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_CROSS_TARGET_SNAPSHOT_H
#define UMICOM_PLATFORM_CROSS_TARGET_CROSS_TARGET_SNAPSHOT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/cross_target_health.h"
/**
 * Represent the ct cross target snapshot data shared with callers of this public contract.
 */
typedef struct UmiCtCrossTargetSnapshot { UmiCtTarget target; char abi[32]; uint64_t cpu_features; uint32_t cpu_count; uint64_t memory_bytes; uint32_t page_size; UmiCtCrossTargetHealth health; uint64_t fingerprint; uint64_t revision; } UmiCtCrossTargetSnapshot;
/**
 * Check that ct cross target snapshot satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ct_cross_target_snapshot_validate(const UmiCtCrossTargetSnapshot *snapshot);

#ifdef __cplusplus
}
#endif

#endif
