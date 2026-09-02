/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/statistics_record.h
 *
 * PURPOSE:
 *   Define the reusable selection statistics record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_STATISTICS_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_STATISTICS_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection statistics record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionStatisticsRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionStatisticsRecord;

/**
 * Initialise workbench selection statistics record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_statistics_record_init(
    UmiWorkbenchSelectionStatisticsRecord *record,
    const char *record_id);
/**
 * Check that workbench selection statistics record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_statistics_record_validate(
    const UmiWorkbenchSelectionStatisticsRecord *record);
/**
 * Provide the workbench selection statistics record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_statistics_record_set_source(
    UmiWorkbenchSelectionStatisticsRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection statistics record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_statistics_record_set_subject(
    UmiWorkbenchSelectionStatisticsRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection statistics record set related operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_statistics_record_set_related(
    UmiWorkbenchSelectionStatisticsRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection statistics record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_statistics_record_set_group(
    UmiWorkbenchSelectionStatisticsRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection statistics record set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_statistics_record_set_description(
    UmiWorkbenchSelectionStatisticsRecord *record,
    const char *description);
/**
 * Provide the workbench selection statistics record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_statistics_record_hash(
    const UmiWorkbenchSelectionStatisticsRecord *record);
/**
 * Provide the workbench selection statistics record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_selection_statistics_record_touch(
    UmiWorkbenchSelectionStatisticsRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
