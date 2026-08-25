/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/regeneration_policy.h
 *
 * PURPOSE:
 *   Define when regeneration is allowed, reviewable, blocked or forced back to manual engineering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_REGENERATION_POLICY_H
#define UMICOM_DEVELOPER_HELIX_REGENERATION_POLICY_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixRegenerationPolicy {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double threshold; bool blocking;
} UmiHelixRegenerationPolicy;
/* Initialise a deterministic, empty regeneration policy record. */
void umi_helix_regeneration_policy_init(UmiHelixRegenerationPolicy *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_regeneration_policy_validate(const UmiHelixRegenerationPolicy *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_regeneration_policy_score(const UmiHelixRegenerationPolicy *record);
#ifdef __cplusplus
}
#endif
#endif
