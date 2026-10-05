/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/panel_header_model.h
 *
 * PURPOSE:
 *   Define the reusable linked-panel header view model contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_PANEL_HEADER_MODEL_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_PANEL_HEADER_MODEL_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link panel header model data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkPanelHeaderModel {
    uint32_t structure_size;
    char model_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkPanelHeaderModel;

/**
 * Initialise workbench context link panel header model from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_link_panel_header_model_init(UmiWorkbenchContextLinkPanelHeaderModel *record,
                                           const char *identity);
/**
 * Check that workbench context link panel header model satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_link_panel_header_model_validate(
    const UmiWorkbenchContextLinkPanelHeaderModel *record);
/**
 * Copy workbench context link panel header model into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_panel_header_model_copy(
    UmiWorkbenchContextLinkPanelHeaderModel *destination,
    const UmiWorkbenchContextLinkPanelHeaderModel *source);
/**
 * Provide the workbench context link panel header model hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_link_panel_header_model_hash(
    const UmiWorkbenchContextLinkPanelHeaderModel *record);
/**
 * Provide the workbench context link panel header model set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_panel_header_model_set_primary(
    UmiWorkbenchContextLinkPanelHeaderModel *record,
    const char *value);
/**
 * Provide the workbench context link panel header model set secondary operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_link_panel_header_model_set_secondary(
    UmiWorkbenchContextLinkPanelHeaderModel *record,
    const char *value);
/**
 * Provide the workbench context link panel header model touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_link_panel_header_model_touch(
    UmiWorkbenchContextLinkPanelHeaderModel *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_panel_header_model_archive_encode(const UmiWorkbenchContextLinkPanelHeaderModel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_panel_header_model_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkPanelHeaderModel *value);

#ifdef __cplusplus
}
#endif

#endif
