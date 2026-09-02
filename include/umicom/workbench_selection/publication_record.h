/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/publication_record.h
 *
 * PURPOSE:
 *   Define the reusable selection publication record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PUBLICATION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PUBLICATION_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection publication record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionPublicationRecord {
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
} UmiWorkbenchSelectionPublicationRecord;

/**
 * Initialise workbench selection publication record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_publication_record_init(
    UmiWorkbenchSelectionPublicationRecord *record,
    const char *record_id);
/**
 * Check that workbench selection publication record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_publication_record_validate(
    const UmiWorkbenchSelectionPublicationRecord *record);
/**
 * Provide the workbench selection publication record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_publication_record_set_source(
    UmiWorkbenchSelectionPublicationRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection publication record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_publication_record_set_subject(
    UmiWorkbenchSelectionPublicationRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection publication record set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_publication_record_set_secondary(
    UmiWorkbenchSelectionPublicationRecord *record,
    const char *secondary_id);
/**
 * Provide the workbench selection publication record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_publication_record_set_group(
    UmiWorkbenchSelectionPublicationRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection publication record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_publication_record_set_label(
    UmiWorkbenchSelectionPublicationRecord *record,
    const char *label);
/**
 * Provide the workbench selection publication record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_selection_publication_record_hash(
    const UmiWorkbenchSelectionPublicationRecord *record);
/**
 * Provide the workbench selection publication record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_selection_publication_record_touch(
    UmiWorkbenchSelectionPublicationRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
