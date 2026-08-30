/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/badge_model.h
 *
 * PURPOSE:
 *   Define the reusable linked-context badge view model contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_BADGE_MODEL_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_BADGE_MODEL_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkBadgeModel {
    uint32_t structure_size;
    char model_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkBadgeModel;

void umi_workbench_context_link_badge_model_init(UmiWorkbenchContextLinkBadgeModel *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_badge_model_validate(
    const UmiWorkbenchContextLinkBadgeModel *record);
UmiStatus umi_workbench_context_link_badge_model_copy(
    UmiWorkbenchContextLinkBadgeModel *destination,
    const UmiWorkbenchContextLinkBadgeModel *source);
uint64_t umi_workbench_context_link_badge_model_hash(
    const UmiWorkbenchContextLinkBadgeModel *record);
UmiStatus umi_workbench_context_link_badge_model_set_primary(
    UmiWorkbenchContextLinkBadgeModel *record,
    const char *value);
UmiStatus umi_workbench_context_link_badge_model_set_secondary(
    UmiWorkbenchContextLinkBadgeModel *record,
    const char *value);
void umi_workbench_context_link_badge_model_touch(
    UmiWorkbenchContextLinkBadgeModel *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
