/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/capability_set.c
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
#include "umicom/frontend/conformance/capability_set.h"

void umi_fc_capability_set_clear(UmiFcCapabilitySet *set) { if (set != NULL) set->bits = 0U; }
UmiStatus umi_fc_capability_set_add(UmiFcCapabilitySet *set, UmiFcCapabilityKind capability) { uint64_t bit; if (set == NULL) return UMI_STATUS_INVALID_ARGUMENT; bit = capability < UMI_FC_CAP_COUNT ? (UINT64_C(1) << (unsigned)capability) : 0U; if (bit == 0U) return UMI_STATUS_INVALID_ARGUMENT; set->bits |= bit; return UMI_STATUS_OK; }
bool umi_fc_capability_set_contains(const UmiFcCapabilitySet *set, UmiFcCapabilityKind capability) { return set != NULL && capability < UMI_FC_CAP_COUNT && (set->bits & (UINT64_C(1) << (unsigned)capability)) != 0U; }
size_t umi_fc_capability_set_count(const UmiFcCapabilitySet *set) { return set == NULL ? 0U : (size_t)__builtin_popcountll(set->bits); }
uint64_t umi_fc_capability_set_missing(const UmiFcCapabilitySet *required, const UmiFcCapabilitySet *actual) { if (required == NULL) return 0U; return required->bits & ~(actual != NULL ? actual->bits : 0U); }
