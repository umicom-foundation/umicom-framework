/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/capabilities.h
 *
 * PURPOSE:
 *   Define the append-only debugger capability bitset used to negotiate the
 *   operations supported by a Debug Adapter Protocol provider or another
 *   Framework debugger adapter.
 *
 * STABILITY:
 *   Capability bit positions form part of the stable public C ABI. Existing
 *   values must never be renumbered or reused. New capabilities are appended
 *   at the next unused bit so older applications can safely ignore them.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_CAPABILITIES_H
#define UMICOM_DEBUG_CAPABILITIES_H

#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

enum UmiDebugCapability {
    UMI_DEBUG_CAP_CONFIGURATION_DONE = 1ULL << 0,
    UMI_DEBUG_CAP_FUNCTION_BREAKPOINTS = 1ULL << 1,
    UMI_DEBUG_CAP_CONDITIONAL_BREAKPOINTS = 1ULL << 2,
    UMI_DEBUG_CAP_HIT_CONDITIONS = 1ULL << 3,
    UMI_DEBUG_CAP_EVALUATE_HOVER = 1ULL << 4,
    UMI_DEBUG_CAP_SET_VARIABLE = 1ULL << 5,
    UMI_DEBUG_CAP_RESTART = 1ULL << 6,
    UMI_DEBUG_CAP_TERMINATE = 1ULL << 7,
    UMI_DEBUG_CAP_MODULES = 1ULL << 8,
    UMI_DEBUG_CAP_READ_MEMORY = 1ULL << 9,
    UMI_DEBUG_CAP_DISASSEMBLE = 1ULL << 10,
    UMI_DEBUG_CAP_STEP_BACK = 1ULL << 11,
    UMI_DEBUG_CAP_DATA_BREAKPOINTS = 1ULL << 12,
    UMI_DEBUG_CAP_INSTRUCTION_BREAKPOINTS = 1ULL << 13,
    UMI_DEBUG_CAP_EXCEPTION_FILTERS = 1ULL << 14,
    UMI_DEBUG_CAP_WRITE_MEMORY = 1ULL << 15,
    UMI_DEBUG_CAP_REGISTERS = 1ULL << 16,
    UMI_DEBUG_CAP_SET_EXPRESSION = 1ULL << 17,
    UMI_DEBUG_CAP_RESTART_FRAME = 1ULL << 18,
    UMI_DEBUG_CAP_REVERSE_CONTINUE = 1ULL << 19
};

typedef struct UmiDebugCapabilitySet {
    uint64_t supported;
    uint64_t advertised;
    uint64_t required;
    uint64_t revision;
} UmiDebugCapabilitySet;

void umi_debug_capability_set_init(UmiDebugCapabilitySet *set);
UmiStatus umi_debug_capability_set_advertise(
    UmiDebugCapabilitySet *set,
    uint64_t capabilities);
UmiStatus umi_debug_capability_set_require(
    UmiDebugCapabilitySet *set,
    uint64_t capabilities);
int umi_debug_capability_set_ready(const UmiDebugCapabilitySet *set);
int umi_debug_capability_set_has(
    const UmiDebugCapabilitySet *set,
    uint64_t capability);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_CAPABILITIES_H */
