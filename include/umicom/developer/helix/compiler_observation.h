/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/compiler_observation.h
 *
 * PURPOSE:
 *   Capture provider, target, diagnostic and compilation-stage evidence from Framework compiler services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_COMPILER_OBSERVATION_H
#define UMICOM_DEVELOPER_HELIX_COMPILER_OBSERVATION_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the helix compiler observation data shared with callers of this public
 * contract.
 */
typedef struct UmiHelixCompilerObservation {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; size_t passed; size_t failed;
} UmiHelixCompilerObservation;
/* Initialise a deterministic, empty compiler observation record. */
void umi_helix_compiler_observation_init(UmiHelixCompilerObservation *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_compiler_observation_validate(const UmiHelixCompilerObservation *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_compiler_observation_score(const UmiHelixCompilerObservation *record);
#ifdef __cplusplus
}
#endif
#endif
