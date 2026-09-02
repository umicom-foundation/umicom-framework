/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/feedback_event.h
 *
 * PURPOSE:
 *   Represent one build/test/compiler/observability feedback event consumed by a Helix loop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_FEEDBACK_EVENT_H
#define UMICOM_DEVELOPER_HELIX_FEEDBACK_EVENT_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the helix feedback event data shared with callers of this public contract.
 */
typedef struct UmiHelixFeedbackEvent {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double risk;
} UmiHelixFeedbackEvent;
/* Initialise a deterministic, empty feedback event record. */
void umi_helix_feedback_event_init(UmiHelixFeedbackEvent *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_feedback_event_validate(const UmiHelixFeedbackEvent *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_feedback_event_score(const UmiHelixFeedbackEvent *record);
#ifdef __cplusplus
}
#endif
#endif
