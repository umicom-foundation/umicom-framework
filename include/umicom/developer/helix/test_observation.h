/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/test_observation.h
 *
 * PURPOSE:
 *   Capture test outcome, pass/fail/skip counts and regression evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_TEST_OBSERVATION_H
#define UMICOM_DEVELOPER_HELIX_TEST_OBSERVATION_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixTestObservation {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; size_t passed; size_t failed;
} UmiHelixTestObservation;
/* Initialise a deterministic, empty test observation record. */
void umi_helix_test_observation_init(UmiHelixTestObservation *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_test_observation_validate(const UmiHelixTestObservation *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_test_observation_score(const UmiHelixTestObservation *record);
#ifdef __cplusplus
}
#endif
#endif
