/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/status_model.h
 *
 * PURPOSE:
 *   Define the reusable context-link status view model contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_STATUS_MODEL_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_STATUS_MODEL_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkStatusModel {
    uint32_t structure_size;
    char model_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char message[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    UmiContextKind context_kind;
    UmiContextChannelColour colour;
    UmiWorkbenchContextLinkMode mode;
    UmiWorkbenchContextLinkState state;
    UmiWorkbenchContextLinkOrigin origin;
    UmiWorkbenchContextLinkPriority priority;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextLinkStatusModel;

void umi_workbench_context_link_status_model_init(UmiWorkbenchContextLinkStatusModel *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_status_model_validate(
    const UmiWorkbenchContextLinkStatusModel *record);
UmiStatus umi_workbench_context_link_status_model_copy(
    UmiWorkbenchContextLinkStatusModel *destination,
    const UmiWorkbenchContextLinkStatusModel *source);
uint64_t umi_workbench_context_link_status_model_hash(
    const UmiWorkbenchContextLinkStatusModel *record);
UmiStatus umi_workbench_context_link_status_model_set_primary(
    UmiWorkbenchContextLinkStatusModel *record,
    const char *value);
UmiStatus umi_workbench_context_link_status_model_set_secondary(
    UmiWorkbenchContextLinkStatusModel *record,
    const char *value);
void umi_workbench_context_link_status_model_touch(
    UmiWorkbenchContextLinkStatusModel *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
