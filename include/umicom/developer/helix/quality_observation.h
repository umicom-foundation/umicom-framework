/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/quality_observation.h
 *
 * PURPOSE:
 *   Capture quality, coverage, complexity or conformance evidence used by release/repair gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_QUALITY_OBSERVATION_H
#define UMICOM_DEVELOPER_HELIX_QUALITY_OBSERVATION_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixQualityObservation {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double quality; double regression;
} UmiHelixQualityObservation;
/* Initialise a deterministic, empty quality observation record. */
void umi_helix_quality_observation_init(UmiHelixQualityObservation *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_quality_observation_validate(const UmiHelixQualityObservation *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_quality_observation_score(const UmiHelixQualityObservation *record);
#ifdef __cplusplus
}
#endif
#endif
