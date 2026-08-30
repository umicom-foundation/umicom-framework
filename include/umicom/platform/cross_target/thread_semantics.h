/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/thread_semantics.h
 *
 * PURPOSE:
 *   Describe TLS, affinity and priority features used by Framework worker and service runtimes.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_THREAD_SEMANTICS_H
#define UMICOM_PLATFORM_CROSS_TARGET_THREAD_SEMANTICS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCtThreadSemantics { bool threads; bool tls; bool affinity; bool priority; bool naming; uint32_t maximum_logical_cpus; } UmiCtThreadSemantics;
UmiCtSupportLevel umi_ct_thread_semantics_support(const UmiCtThreadSemantics *semantics,bool require_affinity,bool require_priority);

#ifdef __cplusplus
}
#endif

#endif
