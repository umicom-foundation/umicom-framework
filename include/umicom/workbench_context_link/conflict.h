/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/conflict.h
 *
 * PURPOSE:
 *   Define the reusable context-link conflict record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_CONFLICT_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_CONFLICT_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkConflict {
    uint32_t structure_size;
    char conflict_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
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
} UmiWorkbenchContextLinkConflict;

void umi_workbench_context_link_conflict_init(UmiWorkbenchContextLinkConflict *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_conflict_validate(
    const UmiWorkbenchContextLinkConflict *record);
UmiStatus umi_workbench_context_link_conflict_copy(
    UmiWorkbenchContextLinkConflict *destination,
    const UmiWorkbenchContextLinkConflict *source);
uint64_t umi_workbench_context_link_conflict_hash(
    const UmiWorkbenchContextLinkConflict *record);
UmiStatus umi_workbench_context_link_conflict_set_primary(
    UmiWorkbenchContextLinkConflict *record,
    const char *value);
UmiStatus umi_workbench_context_link_conflict_set_secondary(
    UmiWorkbenchContextLinkConflict *record,
    const char *value);
void umi_workbench_context_link_conflict_touch(
    UmiWorkbenchContextLinkConflict *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
