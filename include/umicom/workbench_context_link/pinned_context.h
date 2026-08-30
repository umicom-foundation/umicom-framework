/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/pinned_context.h
 *
 * PURPOSE:
 *   Define the reusable pinned context record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_PINNED_CONTEXT_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_PINNED_CONTEXT_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkPinnedContext {
    uint32_t structure_size;
    char pin_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
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
} UmiWorkbenchContextLinkPinnedContext;

void umi_workbench_context_link_pinned_context_init(UmiWorkbenchContextLinkPinnedContext *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_pinned_context_validate(
    const UmiWorkbenchContextLinkPinnedContext *record);
UmiStatus umi_workbench_context_link_pinned_context_copy(
    UmiWorkbenchContextLinkPinnedContext *destination,
    const UmiWorkbenchContextLinkPinnedContext *source);
uint64_t umi_workbench_context_link_pinned_context_hash(
    const UmiWorkbenchContextLinkPinnedContext *record);
UmiStatus umi_workbench_context_link_pinned_context_set_primary(
    UmiWorkbenchContextLinkPinnedContext *record,
    const char *value);
UmiStatus umi_workbench_context_link_pinned_context_set_secondary(
    UmiWorkbenchContextLinkPinnedContext *record,
    const char *value);
void umi_workbench_context_link_pinned_context_touch(
    UmiWorkbenchContextLinkPinnedContext *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
