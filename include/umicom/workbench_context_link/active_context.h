/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/active_context.h
 *
 * PURPOSE:
 *   Define the reusable active group context contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_ACTIVE_CONTEXT_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_ACTIVE_CONTEXT_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkActiveContext {
    uint32_t structure_size;
    char active_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char context_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkActiveContext;

void umi_workbench_context_link_active_context_init(UmiWorkbenchContextLinkActiveContext *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_active_context_validate(
    const UmiWorkbenchContextLinkActiveContext *record);
UmiStatus umi_workbench_context_link_active_context_copy(
    UmiWorkbenchContextLinkActiveContext *destination,
    const UmiWorkbenchContextLinkActiveContext *source);
uint64_t umi_workbench_context_link_active_context_hash(
    const UmiWorkbenchContextLinkActiveContext *record);
UmiStatus umi_workbench_context_link_active_context_set_primary(
    UmiWorkbenchContextLinkActiveContext *record,
    const char *value);
UmiStatus umi_workbench_context_link_active_context_set_secondary(
    UmiWorkbenchContextLinkActiveContext *record,
    const char *value);
void umi_workbench_context_link_active_context_touch(
    UmiWorkbenchContextLinkActiveContext *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
