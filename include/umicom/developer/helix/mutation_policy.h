/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/mutation_policy.h
 *
 * PURPOSE:
 *   Bound the number, size and classes of allowed changes when deriving a new variant.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_MUTATION_POLICY_H
#define UMICOM_DEVELOPER_HELIX_MUTATION_POLICY_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the helix mutation policy data shared with callers of this public contract.
 */
typedef struct UmiHelixMutationPolicy {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; uint64_t limit; uint64_t used;
} UmiHelixMutationPolicy;
/* Initialise a deterministic, empty mutation policy record. */
void umi_helix_mutation_policy_init(UmiHelixMutationPolicy *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_mutation_policy_validate(const UmiHelixMutationPolicy *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_mutation_policy_score(const UmiHelixMutationPolicy *record);
#ifdef __cplusplus
}
#endif
#endif
