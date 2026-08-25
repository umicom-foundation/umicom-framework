/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/regeneration_request.h
 *
 * PURPOSE:
 *   Describe a request to regenerate a bounded software unit from explicit intent and baseline evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_REGENERATION_REQUEST_H
#define UMICOM_DEVELOPER_HELIX_REGENERATION_REQUEST_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixRegenerationRequest {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double confidence;
} UmiHelixRegenerationRequest;
/* Initialise a deterministic, empty regeneration request record. */
void umi_helix_regeneration_request_init(UmiHelixRegenerationRequest *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_regeneration_request_validate(const UmiHelixRegenerationRequest *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_regeneration_request_score(const UmiHelixRegenerationRequest *record);
#ifdef __cplusplus
}
#endif
#endif
