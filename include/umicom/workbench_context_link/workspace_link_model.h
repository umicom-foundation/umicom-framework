/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/workspace_link_model.h
 *
 * PURPOSE:
 *   Define the reusable workspace link summary model contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_WORKSPACE_LINK_MODEL_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_WORKSPACE_LINK_MODEL_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link workspace link model data shared with callers of
 * this public contract.
 */
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

/**
 * Initialise workbench context link workspace link model from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_link_workspace_link_model_init(UmiWorkbenchContextLinkWorkspaceLinkModel *record,
                                           const char *identity);
/**
 * Check that workbench context link workspace link model satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_link_workspace_link_model_validate(
    const UmiWorkbenchContextLinkWorkspaceLinkModel *record);
/**
 * Copy workbench context link workspace link model into module-owned storage so callers
 * keep ownership of their input values.
 */
UmiStatus umi_workbench_context_link_workspace_link_model_copy(
    UmiWorkbenchContextLinkWorkspaceLinkModel *destination,
    const UmiWorkbenchContextLinkWorkspaceLinkModel *source);
/**
 * Provide the workbench context link workspace link model hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_context_link_workspace_link_model_hash(
    const UmiWorkbenchContextLinkWorkspaceLinkModel *record);
/**
 * Provide the workbench context link workspace link model set primary operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_link_workspace_link_model_set_primary(
    UmiWorkbenchContextLinkWorkspaceLinkModel *record,
    const char *value);
/**
 * Provide the workbench context link workspace link model set secondary operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_link_workspace_link_model_set_secondary(
    UmiWorkbenchContextLinkWorkspaceLinkModel *record,
    const char *value);
/**
 * Provide the workbench context link workspace link model touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_link_workspace_link_model_touch(
    UmiWorkbenchContextLinkWorkspaceLinkModel *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
