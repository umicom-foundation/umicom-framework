/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/diff_options.h
 *
 * PURPOSE:
 *   Define deterministic user comparison options shared by every frontend.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_DIFF_OPTIONS_H
#define UMICOM_VCS_ADVANCED_DIFF_OPTIONS_H

#include "umicom/vcs/advanced/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced diff options data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedDiffOptions {
    uint32_t struct_size;
    uint32_t api_version;
    UmiVcsWhitespaceMode whitespace;
    size_t context_lines;
    int ignore_case;
    int detect_moves;
    int semantic;
    int word_diff;
    int treat_crlf_as_lf;
} UmiVcsAdvancedDiffOptions;

/**
 * Initialise vcs advanced diff options from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_diff_options_init(UmiVcsAdvancedDiffOptions *value);
/**
 * Check that vcs advanced diff options satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_diff_options_validate(const UmiVcsAdvancedDiffOptions *value);
/**
 * Provide the vcs advanced diff options fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_vcs_advanced_diff_options_fingerprint(const UmiVcsAdvancedDiffOptions *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_vcs_advanced_diff_options_archive_encode(const UmiVcsAdvancedDiffOptions *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_vcs_advanced_diff_options_archive_decode(const void *bytes, size_t byte_count,
    UmiVcsAdvancedDiffOptions *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiVcsAdvancedDiffOptions object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_vcs_advanced_diff_options_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiVcsAdvancedDiffOptions *workspace, UmiVcsAdvancedDiffOptions *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_vcs_advanced_diff_options_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiVcsAdvancedDiffOptions *expected, UmiVcsAdvancedDiffOptions *value);


#ifdef __cplusplus
}
#endif

#endif
