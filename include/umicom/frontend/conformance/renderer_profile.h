/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/renderer_profile.h
 *
 * PURPOSE:
 *   renderer identity, capability and policy metadata used by conformance evaluation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_RENDERER_PROFILE_H
#define UMICOM_FRONTEND_CONFORMANCE_RENDERER_PROFILE_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc renderer profile data shared with callers of this public contract.
 */
typedef struct UmiFcRendererProfile { char id[UMI_FC_ID_CAPACITY]; UmiFcFrontendKind kind; uint64_t capabilities; uint32_t api_version; bool production_ready; bool remote_session; } UmiFcRendererProfile;
/**
 * Provide the fc renderer profile make operation used by this module and its client
 * applications.
 */
UmiStatus umi_fc_renderer_profile_make(const char *id,UmiFcFrontendKind kind,uint64_t capabilities,uint32_t api_version,UmiFcRendererProfile *out_profile);
/**
 * Check that fc renderer profile satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fc_renderer_profile_validate(const UmiFcRendererProfile *profile);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fc_renderer_profile_archive_encode(const UmiFcRendererProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fc_renderer_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiFcRendererProfile *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiFcRendererProfile object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_fc_renderer_profile_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiFcRendererProfile *workspace, UmiFcRendererProfile *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_fc_renderer_profile_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiFcRendererProfile *expected, UmiFcRendererProfile *value);


#ifdef __cplusplus
}
#endif
#endif
