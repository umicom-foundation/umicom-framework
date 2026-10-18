/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/diagnostic.h
 *
 * PURPOSE:
 *   Define cross-application panel diagnostic state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_DIAGNOSTIC_H
#define UMICOM_CROSS_APPLICATION_PANEL_DIAGNOSTIC_H
#include "umicom/cross_application_panel/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel diagnostic data shared with callers of this public contract.
 */
typedef struct UmiPanelDiagnostic {
    uint32_t structure_size;
    char diagnostic_id[UMI_PANEL_TEXT_CAPACITY];
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char instance_id[UMI_PANEL_TEXT_CAPACITY];
    char message[UMI_PANEL_DESCRIPTION_CAPACITY];
    uint32_t severity;
    UmiStatus status;
    uint64_t revision;
} UmiPanelDiagnostic;
/**
 * Represent the panel diagnostic store data shared with callers of this public contract.
 */
typedef struct UmiPanelDiagnosticStore { UmiPanelDiagnostic items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelDiagnosticStore;
/**
 * Initialise panel diagnostic from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_diagnostic_init(UmiPanelDiagnostic *record);
/**
 * Check that panel diagnostic satisfies its contract before another service relies on it.
 */
UmiStatus umi_panel_diagnostic_validate(const UmiPanelDiagnostic *record);
/**
 * Initialise panel diagnostic store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_diagnostic_store_init(UmiPanelDiagnosticStore *store);
/**
 * Provide the panel diagnostic store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_diagnostic_store_put(UmiPanelDiagnosticStore *store,const UmiPanelDiagnostic *record);
/**
 * Remove panel diagnostic store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_diagnostic_store_remove(UmiPanelDiagnosticStore *store,const char *identity);
/**
 * Find panel diagnostic store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiPanelDiagnostic *umi_panel_diagnostic_store_find(UmiPanelDiagnosticStore *store,const char *identity);
/**
 * Provide the panel diagnostic store find const operation used by this module and its
 * client applications.
 */
const UmiPanelDiagnostic *umi_panel_diagnostic_store_find_const(const UmiPanelDiagnosticStore *store,const char *identity);
/**
 * Provide the panel diagnostic store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_diagnostic_store_snapshot(const UmiPanelDiagnosticStore *store,UmiPanelDiagnostic *records,size_t capacity,size_t *out_count);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_panel_diagnostic_archive_encode(const UmiPanelDiagnostic *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_panel_diagnostic_archive_decode(const void *bytes, size_t byte_count,
    UmiPanelDiagnostic *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiPanelDiagnostic object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_panel_diagnostic_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiPanelDiagnostic *workspace, UmiPanelDiagnostic *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_panel_diagnostic_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiPanelDiagnostic *expected, UmiPanelDiagnostic *value);


#ifdef __cplusplus
}
#endif
#endif
