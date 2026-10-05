/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/field_mapping.h
 *
 * PURPOSE:
 *   Define the reusable selection field mapping contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_FIELD_MAPPING_H
#define UMICOM_WORKBENCH_SELECTION_FIELD_MAPPING_H
#include "umicom/workbench_selection/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection field mapping data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionFieldMapping {
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
} UmiWorkbenchSelectionFieldMapping;

/**
 * Initialise workbench selection field mapping from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_field_mapping_init(
    UmiWorkbenchSelectionFieldMapping *record,
    const char *record_id);
/**
 * Check that workbench selection field mapping satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_field_mapping_validate(
    const UmiWorkbenchSelectionFieldMapping *record);
/**
 * Provide the workbench selection field mapping set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_field_mapping_set_source(
    UmiWorkbenchSelectionFieldMapping *record,
    const char *source_id);
/**
 * Provide the workbench selection field mapping set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_field_mapping_set_subject(
    UmiWorkbenchSelectionFieldMapping *record,
    const char *subject_id);
/**
 * Provide the workbench selection field mapping set related operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_field_mapping_set_related(
    UmiWorkbenchSelectionFieldMapping *record,
    const char *related_id);
/**
 * Provide the workbench selection field mapping set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_field_mapping_set_group(
    UmiWorkbenchSelectionFieldMapping *record,
    const char *group_id);
/**
 * Provide the workbench selection field mapping set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_field_mapping_set_description(
    UmiWorkbenchSelectionFieldMapping *record,
    const char *description);
/**
 * Provide the workbench selection field mapping hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_field_mapping_hash(
    const UmiWorkbenchSelectionFieldMapping *record);
/**
 * Provide the workbench selection field mapping touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_field_mapping_touch(
    UmiWorkbenchSelectionFieldMapping *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_selection_field_mapping_archive_encode(const UmiWorkbenchSelectionFieldMapping *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_selection_field_mapping_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchSelectionFieldMapping *value);

#ifdef __cplusplus
}
#endif
#endif
