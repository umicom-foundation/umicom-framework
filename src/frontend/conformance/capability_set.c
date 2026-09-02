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

/*
 * Release or reset state held by fc capability set so the same storage can be reused
 * safely.
 */
void umi_fc_capability_set_clear(UmiFcCapabilitySet *set) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (set != NULL) set->bits = 0U; }
/* Add fc capability set only after its inputs and available capacity have been checked. */
UmiStatus umi_fc_capability_set_add(UmiFcCapabilitySet *set, UmiFcCapabilityKind capability) { uint64_t bit; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (set == NULL) return UMI_STATUS_INVALID_ARGUMENT; bit = capability < UMI_FC_CAP_COUNT ? (UINT64_C(1) << (unsigned)capability) : 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (bit == 0U) return UMI_STATUS_INVALID_ARGUMENT; set->bits |= bit; return UMI_STATUS_OK; }
/*
 * Provide the fc capability set contains operation used by this module and its client
 * applications.
 */
bool umi_fc_capability_set_contains(const UmiFcCapabilitySet *set, UmiFcCapabilityKind capability) { return set != NULL && capability < UMI_FC_CAP_COUNT && (set->bits & (UINT64_C(1) << (unsigned)capability)) != 0U; }
/*
 * Return the number of records represented by fc capability set without changing their
 * state.
 */
size_t umi_fc_capability_set_count(const UmiFcCapabilitySet *set) { return set == NULL ? 0U : (size_t)__builtin_popcountll(set->bits); }
/*
 * Provide the fc capability set missing operation used by this module and its client
 * applications.
 */
uint64_t umi_fc_capability_set_missing(const UmiFcCapabilitySet *required, const UmiFcCapabilitySet *actual) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (required == NULL) return 0U; return required->bits & ~(actual != NULL ? actual->bits : 0U); }
