/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/metric.h
 *
 * PURPOSE:
 *   Define the reusable context-link metric sample contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_METRIC_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_METRIC_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkMetric {
    uint32_t structure_size;
    char metric_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char name[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char unit[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkMetric;

void umi_workbench_context_link_metric_init(UmiWorkbenchContextLinkMetric *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_metric_validate(
    const UmiWorkbenchContextLinkMetric *record);
UmiStatus umi_workbench_context_link_metric_copy(
    UmiWorkbenchContextLinkMetric *destination,
    const UmiWorkbenchContextLinkMetric *source);
uint64_t umi_workbench_context_link_metric_hash(
    const UmiWorkbenchContextLinkMetric *record);
UmiStatus umi_workbench_context_link_metric_set_primary(
    UmiWorkbenchContextLinkMetric *record,
    const char *value);
UmiStatus umi_workbench_context_link_metric_set_secondary(
    UmiWorkbenchContextLinkMetric *record,
    const char *value);
void umi_workbench_context_link_metric_touch(
    UmiWorkbenchContextLinkMetric *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
