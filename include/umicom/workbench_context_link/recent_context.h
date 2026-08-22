/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/recent_context.h
 *
 * PURPOSE:
 *   Define the reusable recent context record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_RECENT_CONTEXT_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_RECENT_CONTEXT_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkRecentContext {
    uint32_t structure_size;
    char recent_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
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
} UmiWorkbenchContextLinkRecentContext;

void umi_workbench_context_link_recent_context_init(UmiWorkbenchContextLinkRecentContext *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_recent_context_validate(
    const UmiWorkbenchContextLinkRecentContext *record);
UmiStatus umi_workbench_context_link_recent_context_copy(
    UmiWorkbenchContextLinkRecentContext *destination,
    const UmiWorkbenchContextLinkRecentContext *source);
uint64_t umi_workbench_context_link_recent_context_hash(
    const UmiWorkbenchContextLinkRecentContext *record);
UmiStatus umi_workbench_context_link_recent_context_set_primary(
    UmiWorkbenchContextLinkRecentContext *record,
    const char *value);
UmiStatus umi_workbench_context_link_recent_context_set_secondary(
    UmiWorkbenchContextLinkRecentContext *record,
    const char *value);
void umi_workbench_context_link_recent_context_touch(
    UmiWorkbenchContextLinkRecentContext *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
