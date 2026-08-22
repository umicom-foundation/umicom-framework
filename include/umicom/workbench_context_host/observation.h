/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/observation.h
 *
 * PURPOSE:
 *   Represent toolkit-neutral workbench focus and selection observations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_OBSERVATION_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_OBSERVATION_H
#include "umicom/workbench_context_host/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextHostObservation {
    uint32_t structure_size;
    char workbench_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char workspace_profile[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char perspective_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char document_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char activity_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char view_container_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    uint64_t source_revision;
    uint64_t observed_at_ms;
} UmiWorkbenchContextHostObservation;
void umi_workbench_context_host_observation_init(
    UmiWorkbenchContextHostObservation *observation);
uint32_t umi_workbench_context_host_observation_diff(
    const UmiWorkbenchContextHostObservation *previous,
    const UmiWorkbenchContextHostObservation *current);
bool umi_workbench_context_host_observation_equal(
    const UmiWorkbenchContextHostObservation *left,
    const UmiWorkbenchContextHostObservation *right);
#ifdef __cplusplus
}
#endif
#endif
