/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/workspace_link_model.h
 *
 * PURPOSE:
 *   Define the reusable workspace link summary model contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_WORKSPACE_LINK_MODEL_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_WORKSPACE_LINK_MODEL_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkWorkspaceLinkModel {
    uint32_t structure_size;
    char model_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char workspace_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkWorkspaceLinkModel;

void umi_workbench_context_link_workspace_link_model_init(UmiWorkbenchContextLinkWorkspaceLinkModel *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_workspace_link_model_validate(
    const UmiWorkbenchContextLinkWorkspaceLinkModel *record);
UmiStatus umi_workbench_context_link_workspace_link_model_copy(
    UmiWorkbenchContextLinkWorkspaceLinkModel *destination,
    const UmiWorkbenchContextLinkWorkspaceLinkModel *source);
uint64_t umi_workbench_context_link_workspace_link_model_hash(
    const UmiWorkbenchContextLinkWorkspaceLinkModel *record);
UmiStatus umi_workbench_context_link_workspace_link_model_set_primary(
    UmiWorkbenchContextLinkWorkspaceLinkModel *record,
    const char *value);
UmiStatus umi_workbench_context_link_workspace_link_model_set_secondary(
    UmiWorkbenchContextLinkWorkspaceLinkModel *record,
    const char *value);
void umi_workbench_context_link_workspace_link_model_touch(
    UmiWorkbenchContextLinkWorkspaceLinkModel *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
