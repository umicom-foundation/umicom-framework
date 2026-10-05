/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/metric.h
 *
 * PURPOSE:
 *   Define the reusable context-link metric sample contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_METRIC_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_METRIC_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link metric data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise workbench context link metric from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_context_link_metric_init(UmiWorkbenchContextLinkMetric *record,
                                           const char *identity);
/**
 * Check that workbench context link metric satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_link_metric_validate(
    const UmiWorkbenchContextLinkMetric *record);
/**
 * Copy workbench context link metric into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiStatus umi_workbench_context_link_metric_copy(
    UmiWorkbenchContextLinkMetric *destination,
    const UmiWorkbenchContextLinkMetric *source);
/**
 * Provide the workbench context link metric hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_link_metric_hash(
    const UmiWorkbenchContextLinkMetric *record);
/**
 * Provide the workbench context link metric set primary operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_link_metric_set_primary(
    UmiWorkbenchContextLinkMetric *record,
    const char *value);
/**
 * Provide the workbench context link metric set secondary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_metric_set_secondary(
    UmiWorkbenchContextLinkMetric *record,
    const char *value);
/**
 * Provide the workbench context link metric touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_link_metric_touch(
    UmiWorkbenchContextLinkMetric *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_metric_archive_encode(const UmiWorkbenchContextLinkMetric *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_metric_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkMetric *value);

#ifdef __cplusplus
}
#endif

#endif
