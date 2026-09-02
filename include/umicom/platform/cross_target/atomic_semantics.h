/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/atomic_semantics.h
 *
 * PURPOSE:
 *   Describe lock-free atomic widths and memory ordering guarantees for cross-target lock-free algorithms.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_ATOMIC_SEMANTICS_H
#define UMICOM_PLATFORM_CROSS_TARGET_ATOMIC_SEMANTICS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ct atomic semantics data shared with callers of this public contract.
 */
typedef struct UmiCtAtomicSemantics { bool width8; bool width16; bool width32; bool width64; bool width128; bool acquire_release; bool sequential_consistency; } UmiCtAtomicSemantics;
/**
 * Provide the ct atomic width lock free operation used by this module and its client
 * applications.
 */
bool umi_ct_atomic_width_lock_free(const UmiCtAtomicSemantics *semantics,uint32_t bits);
/**
 * Provide the ct atomic semantics support operation used by this module and its client
 * applications.
 */
UmiCtSupportLevel umi_ct_atomic_semantics_support(const UmiCtAtomicSemantics *semantics,uint32_t required_bits,bool require_seq_cst);

#ifdef __cplusplus
}
#endif

#endif
