/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/capability_set.h
 *
 * PURPOSE:
 *   bounded capability-set operations for renderer and surface comparisons.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_CAPABILITY_SET_H
#define UMICOM_FRONTEND_CONFORMANCE_CAPABILITY_SET_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc capability set data shared with callers of this public contract.
 */
typedef struct UmiFcCapabilitySet { uint64_t bits; } UmiFcCapabilitySet;
/**
 * Release or reset state held by fc capability set so the same storage can be reused
 * safely.
 */
void umi_fc_capability_set_clear(UmiFcCapabilitySet *set);
/**
 * Add fc capability set only after its inputs and available capacity have been checked.
 */
UmiStatus umi_fc_capability_set_add(UmiFcCapabilitySet *set, UmiFcCapabilityKind capability);
/**
 * Provide the fc capability set contains operation used by this module and its client
 * applications.
 */
bool umi_fc_capability_set_contains(const UmiFcCapabilitySet *set, UmiFcCapabilityKind capability);
/**
 * Return the number of records represented by fc capability set without changing their
 * state.
 */
size_t umi_fc_capability_set_count(const UmiFcCapabilitySet *set);
/**
 * Provide the fc capability set missing operation used by this module and its client
 * applications.
 */
uint64_t umi_fc_capability_set_missing(const UmiFcCapabilitySet *required, const UmiFcCapabilitySet *actual);

#ifdef __cplusplus
}
#endif
#endif
