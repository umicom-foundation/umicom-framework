/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/metric.h
 *
 * PURPOSE:
 *   Define the reusable source metric contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_METRIC_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_METRIC_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source metric data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextSourceMetric {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceMetric;

/**
 * Initialise workbench context source metric from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_metric_init(
    UmiWorkbenchContextSourceMetric *record,
    const char *record_id);
/**
 * Check that workbench context source metric satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_source_metric_validate(
    const UmiWorkbenchContextSourceMetric *record);
/**
 * Provide the workbench context source metric set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_metric_set_source(
    UmiWorkbenchContextSourceMetric *record,
    const char *source_id);
/**
 * Provide the workbench context source metric set panel operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_metric_set_panel(
    UmiWorkbenchContextSourceMetric *record,
    const char *panel_id);
/**
 * Provide the workbench context source metric set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_metric_set_subject(
    UmiWorkbenchContextSourceMetric *record,
    const char *subject_id);
/**
 * Provide the workbench context source metric set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_metric_set_group(
    UmiWorkbenchContextSourceMetric *record,
    const char *group_id);
/**
 * Provide the workbench context source metric set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_metric_set_label(
    UmiWorkbenchContextSourceMetric *record,
    const char *label);
/**
 * Provide the workbench context source metric hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_source_metric_hash(
    const UmiWorkbenchContextSourceMetric *record);
/**
 * Provide the workbench context source metric touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_source_metric_touch(
    UmiWorkbenchContextSourceMetric *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
