/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/build_observation.h
 *
 * PURPOSE:
 *   Capture build outcome, warning/error counts, duration and artifact evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_BUILD_OBSERVATION_H
#define UMICOM_DEVELOPER_HELIX_BUILD_OBSERVATION_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the helix build observation data shared with callers of this public contract.
 */
typedef struct UmiHelixBuildObservation {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; size_t passed; size_t failed;
} UmiHelixBuildObservation;
/* Initialise a deterministic, empty build observation record. */
void umi_helix_build_observation_init(UmiHelixBuildObservation *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_build_observation_validate(const UmiHelixBuildObservation *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_build_observation_score(const UmiHelixBuildObservation *record);
#ifdef __cplusplus
}
#endif
#endif
