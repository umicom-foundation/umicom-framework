/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/change_request.h
 *
 * PURPOSE:
 *   Describe one proposed repository change and the evidence needed before it can be applied.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_CHANGE_REQUEST_H
#define UMICOM_DEVELOPER_HELIX_CHANGE_REQUEST_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixChangeRequest {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double risk;
} UmiHelixChangeRequest;
/* Initialise a deterministic, empty change request record. */
void umi_helix_change_request_init(UmiHelixChangeRequest *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_change_request_validate(const UmiHelixChangeRequest *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_change_request_score(const UmiHelixChangeRequest *record);
#ifdef __cplusplus
}
#endif
#endif
