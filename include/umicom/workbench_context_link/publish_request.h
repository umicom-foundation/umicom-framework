/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/publish_request.h
 *
 * PURPOSE:
 *   Define the reusable context publication request contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_PUBLISH_REQUEST_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_PUBLISH_REQUEST_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link publish request data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkPublishRequest {
    uint32_t structure_size;
    char request_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char source_panel_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkPublishRequest;

/**
 * Initialise workbench context link publish request from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_publish_request_init(UmiWorkbenchContextLinkPublishRequest *record,
                                           const char *identity);
/**
 * Check that workbench context link publish request satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_publish_request_validate(
    const UmiWorkbenchContextLinkPublishRequest *record);
/**
 * Copy workbench context link publish request into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_publish_request_copy(
    UmiWorkbenchContextLinkPublishRequest *destination,
    const UmiWorkbenchContextLinkPublishRequest *source);
/**
 * Provide the workbench context link publish request hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_link_publish_request_hash(
    const UmiWorkbenchContextLinkPublishRequest *record);
/**
 * Provide the workbench context link publish request set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_publish_request_set_primary(
    UmiWorkbenchContextLinkPublishRequest *record,
    const char *value);
/**
 * Provide the workbench context link publish request set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_publish_request_set_secondary(
    UmiWorkbenchContextLinkPublishRequest *record,
    const char *value);
/**
 * Provide the workbench context link publish request touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_publish_request_touch(
    UmiWorkbenchContextLinkPublishRequest *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_publish_request_archive_encode(const UmiWorkbenchContextLinkPublishRequest *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_publish_request_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkPublishRequest *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiWorkbenchContextLinkPublishRequest object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_workbench_context_link_publish_request_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkPublishRequest *workspace, UmiWorkbenchContextLinkPublishRequest *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_workbench_context_link_publish_request_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiWorkbenchContextLinkPublishRequest *expected, UmiWorkbenchContextLinkPublishRequest *value);


/** Replace this inline value only while the observed revision is current.
 * Keep the same identity and a valid domain value. On failure nothing changes;
 * successful publication advances the live revision once. The proposal token
 * does not grant authority. The caller serializes access to the owner. */
UmiStatus umi_workbench_context_link_publish_request_replace_if_current(UmiWorkbenchContextLinkPublishRequest *value,
    uint64_t expected_revision, const UmiWorkbenchContextLinkPublishRequest *proposal);

#ifdef __cplusplus
}
#endif

#endif
