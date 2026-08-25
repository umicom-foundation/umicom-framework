/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/engineering_intent.h
 *
 * PURPOSE:
 *   Capture the requested engineering intent, constraints and acceptance threshold without executing changes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_ENGINEERING_INTENT_H
#define UMICOM_DEVELOPER_HELIX_ENGINEERING_INTENT_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixEngineeringIntent {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double confidence;
} UmiHelixEngineeringIntent;
/* Initialise a deterministic, empty engineering intent record. */
void umi_helix_engineering_intent_init(UmiHelixEngineeringIntent *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_engineering_intent_validate(const UmiHelixEngineeringIntent *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_engineering_intent_score(const UmiHelixEngineeringIntent *record);
#ifdef __cplusplus
}
#endif
#endif
