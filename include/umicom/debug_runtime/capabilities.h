/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/capabilities.h
 *
 * PURPOSE:
 *   Map DAP initialize capabilities onto the existing append-only Framework
 *   UmiDebugCapability bitset used by adapter selection and command gating.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_CAPABILITIES_H
#define UMICOM_DEBUG_RUNTIME_CAPABILITIES_H
#include "umicom/debug/capabilities.h"
#include "umicom/debug_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
uint64_t umi_debug_runtime_capability_bits(
    const UmiDebugRuntimeCapabilities *capabilities);
#ifdef __cplusplus
}
#endif
#endif
