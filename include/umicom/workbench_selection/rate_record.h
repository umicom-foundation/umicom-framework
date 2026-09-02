/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/rate_record.h
 *
 * PURPOSE:
 *   Define the reusable selection rate record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_RATE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_RATE_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection rate record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionRateRecord {
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
} UmiWorkbenchSelectionRateRecord;

/**
 * Initialise workbench selection rate record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_rate_record_init(
    UmiWorkbenchSelectionRateRecord *record,
    const char *record_id);
/**
 * Check that workbench selection rate record satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_selection_rate_record_validate(
    const UmiWorkbenchSelectionRateRecord *record);
/**
 * Provide the workbench selection rate record set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_rate_record_set_source(
    UmiWorkbenchSelectionRateRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection rate record set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_rate_record_set_subject(
    UmiWorkbenchSelectionRateRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection rate record set related operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_rate_record_set_related(
    UmiWorkbenchSelectionRateRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection rate record set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_rate_record_set_group(
    UmiWorkbenchSelectionRateRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection rate record set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_rate_record_set_description(
    UmiWorkbenchSelectionRateRecord *record,
    const char *description);
/**
 * Provide the workbench selection rate record hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_rate_record_hash(
    const UmiWorkbenchSelectionRateRecord *record);
/**
 * Provide the workbench selection rate record touch operation used by this module and its
 * client applications.
 */
void umi_workbench_selection_rate_record_touch(
    UmiWorkbenchSelectionRateRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
