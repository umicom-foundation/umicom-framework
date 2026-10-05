/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/badge_model.h
 *
 * PURPOSE:
 *   Define the reusable linked-context badge view model contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_BADGE_MODEL_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_BADGE_MODEL_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link badge model data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextLinkBadgeModel {
    uint32_t structure_size;
    char model_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkBadgeModel;

/**
 * Initialise workbench context link badge model from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_badge_model_init(UmiWorkbenchContextLinkBadgeModel *record,
                                           const char *identity);
/**
 * Check that workbench context link badge model satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_badge_model_validate(
    const UmiWorkbenchContextLinkBadgeModel *record);
/**
 * Copy workbench context link badge model into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_badge_model_copy(
    UmiWorkbenchContextLinkBadgeModel *destination,
    const UmiWorkbenchContextLinkBadgeModel *source);
/**
 * Provide the workbench context link badge model hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_badge_model_hash(
    const UmiWorkbenchContextLinkBadgeModel *record);
/**
 * Provide the workbench context link badge model set primary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_badge_model_set_primary(
    UmiWorkbenchContextLinkBadgeModel *record,
    const char *value);
/**
 * Provide the workbench context link badge model set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_badge_model_set_secondary(
    UmiWorkbenchContextLinkBadgeModel *record,
    const char *value);
/**
 * Provide the workbench context link badge model touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_badge_model_touch(
    UmiWorkbenchContextLinkBadgeModel *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_badge_model_archive_encode(const UmiWorkbenchContextLinkBadgeModel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_badge_model_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkBadgeModel *value);

#ifdef __cplusplus
}
#endif

#endif
