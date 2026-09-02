/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/target_fingerprint.h
 *
 * PURPOSE:
 *   Compute deterministic target/runtime fingerprints for cache, release and conformance evidence.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_TARGET_FINGERPRINT_H
#define UMICOM_PLATFORM_CROSS_TARGET_TARGET_FINGERPRINT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ct target fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_ct_target_fingerprint(const UmiCtTarget *target,uint64_t cpu_features,const char *abi,uint32_t page_size);

#ifdef __cplusplus
}
#endif

#endif
