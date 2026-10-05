/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/session.h
 *
 * PURPOSE:
 *   Define the reusable context-link workbench session contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_SESSION_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_SESSION_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link session data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextLinkSession {
    uint32_t structure_size;
    char session_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char workspace_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char layout_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkSession;

/**
 * Initialise workbench context link session from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_session_init(UmiWorkbenchContextLinkSession *record,
                                           const char *identity);
/**
 * Check that workbench context link session satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_link_session_validate(
    const UmiWorkbenchContextLinkSession *record);
/**
 * Copy workbench context link session into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiStatus umi_workbench_context_link_session_copy(
    UmiWorkbenchContextLinkSession *destination,
    const UmiWorkbenchContextLinkSession *source);
/**
 * Provide the workbench context link session hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_link_session_hash(
    const UmiWorkbenchContextLinkSession *record);
/**
 * Provide the workbench context link session set primary operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_link_session_set_primary(
    UmiWorkbenchContextLinkSession *record,
    const char *value);
/**
 * Provide the workbench context link session set secondary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_session_set_secondary(
    UmiWorkbenchContextLinkSession *record,
    const char *value);
/**
 * Provide the workbench context link session touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_link_session_touch(
    UmiWorkbenchContextLinkSession *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_session_archive_encode(const UmiWorkbenchContextLinkSession *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_session_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkSession *value);

#ifdef __cplusplus
}
#endif

#endif
