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

typedef struct UmiFcCapabilitySet { uint64_t bits; } UmiFcCapabilitySet;
void umi_fc_capability_set_clear(UmiFcCapabilitySet *set);
UmiStatus umi_fc_capability_set_add(UmiFcCapabilitySet *set, UmiFcCapabilityKind capability);
bool umi_fc_capability_set_contains(const UmiFcCapabilitySet *set, UmiFcCapabilityKind capability);
size_t umi_fc_capability_set_count(const UmiFcCapabilitySet *set);
uint64_t umi_fc_capability_set_missing(const UmiFcCapabilitySet *required, const UmiFcCapabilitySet *actual);

#ifdef __cplusplus
}
#endif
#endif
