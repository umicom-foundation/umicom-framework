/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/import_record.h
 *
 * PURPOSE:
 *   Define the reusable context-link import record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_IMPORT_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_IMPORT_RECORD_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link import record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkImportRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char source_uri[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char workspace_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkImportRecord;

/**
 * Initialise workbench context link import record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_import_record_init(UmiWorkbenchContextLinkImportRecord *record,
                                           const char *identity);
/**
 * Check that workbench context link import record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_import_record_validate(
    const UmiWorkbenchContextLinkImportRecord *record);
/**
 * Copy workbench context link import record into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_import_record_copy(
    UmiWorkbenchContextLinkImportRecord *destination,
    const UmiWorkbenchContextLinkImportRecord *source);
/**
 * Provide the workbench context link import record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_import_record_hash(
    const UmiWorkbenchContextLinkImportRecord *record);
/**
 * Provide the workbench context link import record set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_import_record_set_primary(
    UmiWorkbenchContextLinkImportRecord *record,
    const char *value);
/**
 * Provide the workbench context link import record set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_import_record_set_secondary(
    UmiWorkbenchContextLinkImportRecord *record,
    const char *value);
/**
 * Provide the workbench context link import record touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_import_record_touch(
    UmiWorkbenchContextLinkImportRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
