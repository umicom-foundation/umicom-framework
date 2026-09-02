/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/failure_classification.h
 *
 * PURPOSE:
 *   Classify observed failures into build, test, semantic, infrastructure or policy categories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_FAILURE_CLASSIFICATION_H
#define UMICOM_DEVELOPER_HELIX_FAILURE_CLASSIFICATION_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the helix failure classification data shared with callers of this public
 * contract.
 */
typedef struct UmiHelixFailureClassification {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double confidence;
} UmiHelixFailureClassification;
/* Initialise a deterministic, empty failure classification record. */
void umi_helix_failure_classification_init(UmiHelixFailureClassification *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_failure_classification_validate(const UmiHelixFailureClassification *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_failure_classification_score(const UmiHelixFailureClassification *record);
#ifdef __cplusplus
}
#endif
#endif
