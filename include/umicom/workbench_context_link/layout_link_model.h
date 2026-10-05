/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/layout_link_model.h
 *
 * PURPOSE:
 *   Define the reusable layout link summary model contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_LAYOUT_LINK_MODEL_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_LAYOUT_LINK_MODEL_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link layout link model data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkLayoutLinkModel {
    uint32_t structure_size;
    char model_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char layout_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkLayoutLinkModel;

/**
 * Initialise workbench context link layout link model from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_layout_link_model_init(UmiWorkbenchContextLinkLayoutLinkModel *record,
                                           const char *identity);
/**
 * Check that workbench context link layout link model satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_link_layout_link_model_validate(
    const UmiWorkbenchContextLinkLayoutLinkModel *record);
/**
 * Copy workbench context link layout link model into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_layout_link_model_copy(
    UmiWorkbenchContextLinkLayoutLinkModel *destination,
    const UmiWorkbenchContextLinkLayoutLinkModel *source);
/**
 * Provide the workbench context link layout link model hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_link_layout_link_model_hash(
    const UmiWorkbenchContextLinkLayoutLinkModel *record);
/**
 * Provide the workbench context link layout link model set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_layout_link_model_set_primary(
    UmiWorkbenchContextLinkLayoutLinkModel *record,
    const char *value);
/**
 * Provide the workbench context link layout link model set secondary operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_link_layout_link_model_set_secondary(
    UmiWorkbenchContextLinkLayoutLinkModel *record,
    const char *value);
/**
 * Provide the workbench context link layout link model touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_layout_link_model_touch(
    UmiWorkbenchContextLinkLayoutLinkModel *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_layout_link_model_archive_encode(const UmiWorkbenchContextLinkLayoutLinkModel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_layout_link_model_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkLayoutLinkModel *value);

#ifdef __cplusplus
}
#endif

#endif
