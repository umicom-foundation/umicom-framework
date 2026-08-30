/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/retry_policy.h
 *
 * PURPOSE:
 *   Bound repair/regeneration retries with backoff and mandatory escalation after exhaustion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_RETRY_POLICY_H
#define UMICOM_DEVELOPER_HELIX_RETRY_POLICY_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixRetryPolicy {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; uint64_t limit; uint64_t used;
} UmiHelixRetryPolicy;
/* Initialise a deterministic, empty retry policy record. */
void umi_helix_retry_policy_init(UmiHelixRetryPolicy *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_retry_policy_validate(const UmiHelixRetryPolicy *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_retry_policy_score(const UmiHelixRetryPolicy *record);
#ifdef __cplusplus
}
#endif
#endif
