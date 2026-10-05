/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/group_profile.h
 *
 * PURPOSE:
 *   Define the reusable group profile contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_GROUP_PROFILE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_GROUP_PROFILE_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link group profile data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkGroupProfile {
    uint32_t structure_size;
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char title[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char owner_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkGroupProfile;

/**
 * Initialise workbench context link group profile from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_group_profile_init(UmiWorkbenchContextLinkGroupProfile *record,
                                           const char *identity);
/**
 * Check that workbench context link group profile satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_group_profile_validate(
    const UmiWorkbenchContextLinkGroupProfile *record);
/**
 * Copy workbench context link group profile into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_group_profile_copy(
    UmiWorkbenchContextLinkGroupProfile *destination,
    const UmiWorkbenchContextLinkGroupProfile *source);
/**
 * Provide the workbench context link group profile hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_group_profile_hash(
    const UmiWorkbenchContextLinkGroupProfile *record);
/**
 * Provide the workbench context link group profile set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_group_profile_set_primary(
    UmiWorkbenchContextLinkGroupProfile *record,
    const char *value);
/**
 * Provide the workbench context link group profile set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_group_profile_set_secondary(
    UmiWorkbenchContextLinkGroupProfile *record,
    const char *value);
/**
 * Provide the workbench context link group profile touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_group_profile_touch(
    UmiWorkbenchContextLinkGroupProfile *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_group_profile_archive_encode(const UmiWorkbenchContextLinkGroupProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_group_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkGroupProfile *value);

#ifdef __cplusplus
}
#endif

#endif
