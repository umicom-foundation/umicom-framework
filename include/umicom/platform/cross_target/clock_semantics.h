/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/clock_semantics.h
 *
 * PURPOSE:
 *   Describe monotonic/wall-clock availability and timer resolution for scheduler and profiling portability.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_CLOCK_SEMANTICS_H
#define UMICOM_PLATFORM_CROSS_TARGET_CLOCK_SEMANTICS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ct clock semantics data shared with callers of this public contract.
 */
typedef struct UmiCtClockSemantics { bool monotonic; bool wall_clock; bool high_resolution; uint64_t frequency_hz; uint64_t resolution_ns; } UmiCtClockSemantics;
/**
 * Check that ct clock semantics satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_clock_semantics_validate(const UmiCtClockSemantics *semantics);
/**
 * Provide the ct clock ticks to ns operation used by this module and its client
 * applications.
 */
uint64_t umi_ct_clock_ticks_to_ns(const UmiCtClockSemantics *semantics,uint64_t ticks);

#ifdef __cplusplus
}
#endif

#endif
