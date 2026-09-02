/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/consumer_record.h
 *
 * PURPOSE:
 *   Define the reusable selection consumer record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_CONSUMER_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_CONSUMER_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection consumer record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionConsumerRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char secondary_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char label[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionConsumerRecord;

/**
 * Initialise workbench selection consumer record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_consumer_record_init(
    UmiWorkbenchSelectionConsumerRecord *record,
    const char *record_id);
/**
 * Check that workbench selection consumer record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_consumer_record_validate(
    const UmiWorkbenchSelectionConsumerRecord *record);
/**
 * Provide the workbench selection consumer record set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_consumer_record_set_source(
    UmiWorkbenchSelectionConsumerRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection consumer record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_consumer_record_set_subject(
    UmiWorkbenchSelectionConsumerRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection consumer record set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_consumer_record_set_secondary(
    UmiWorkbenchSelectionConsumerRecord *record,
    const char *secondary_id);
/**
 * Provide the workbench selection consumer record set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_consumer_record_set_group(
    UmiWorkbenchSelectionConsumerRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection consumer record set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_consumer_record_set_label(
    UmiWorkbenchSelectionConsumerRecord *record,
    const char *label);
/**
 * Provide the workbench selection consumer record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_consumer_record_hash(
    const UmiWorkbenchSelectionConsumerRecord *record);
/**
 * Provide the workbench selection consumer record touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_consumer_record_touch(
    UmiWorkbenchSelectionConsumerRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
