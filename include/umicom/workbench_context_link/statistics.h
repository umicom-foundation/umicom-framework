/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/statistics.h
 *
 * PURPOSE:
 *   Define the reusable context-link statistics record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_STATISTICS_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_STATISTICS_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkStatistics {
    uint32_t structure_size;
    char statistics_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char scope_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkStatistics;

void umi_workbench_context_link_statistics_init(UmiWorkbenchContextLinkStatistics *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_statistics_validate(
    const UmiWorkbenchContextLinkStatistics *record);
UmiStatus umi_workbench_context_link_statistics_copy(
    UmiWorkbenchContextLinkStatistics *destination,
    const UmiWorkbenchContextLinkStatistics *source);
uint64_t umi_workbench_context_link_statistics_hash(
    const UmiWorkbenchContextLinkStatistics *record);
UmiStatus umi_workbench_context_link_statistics_set_primary(
    UmiWorkbenchContextLinkStatistics *record,
    const char *value);
UmiStatus umi_workbench_context_link_statistics_set_secondary(
    UmiWorkbenchContextLinkStatistics *record,
    const char *value);
void umi_workbench_context_link_statistics_touch(
    UmiWorkbenchContextLinkStatistics *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
