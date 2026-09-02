/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/risk_assessment.h
 *
 * PURPOSE:
 *   Aggregate change, build, test and repository evidence into a reviewable engineering risk score.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_RISK_ASSESSMENT_H
#define UMICOM_DEVELOPER_HELIX_RISK_ASSESSMENT_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the helix risk assessment data shared with callers of this public contract.
 */
typedef struct UmiHelixRiskAssessment {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double risk;
} UmiHelixRiskAssessment;
/* Initialise a deterministic, empty risk assessment record. */
void umi_helix_risk_assessment_init(UmiHelixRiskAssessment *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_risk_assessment_validate(const UmiHelixRiskAssessment *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_risk_assessment_score(const UmiHelixRiskAssessment *record);
#ifdef __cplusplus
}
#endif
#endif
