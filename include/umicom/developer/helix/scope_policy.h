/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/scope_policy.h
 *
 * PURPOSE:
 *   Constrain autonomous engineering work to explicitly permitted repository, path and operation scope.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_SCOPE_POLICY_H
#define UMICOM_DEVELOPER_HELIX_SCOPE_POLICY_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixScopePolicy {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double threshold; bool blocking;
} UmiHelixScopePolicy;
/* Initialise a deterministic, empty scope policy record. */
void umi_helix_scope_policy_init(UmiHelixScopePolicy *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_scope_policy_validate(const UmiHelixScopePolicy *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_scope_policy_score(const UmiHelixScopePolicy *record);
#ifdef __cplusplus
}
#endif
#endif
