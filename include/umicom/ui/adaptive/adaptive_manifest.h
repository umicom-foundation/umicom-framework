/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/adaptive_manifest.h
 *
 * PURPOSE:
 *   Declare application-wide adaptive shell capabilities and renderer coverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_ADAPTIVE_MANIFEST_H
#define UMICOM_UI_ADAPTIVE_ADAPTIVE_MANIFEST_H
#include "umicom/ui/adaptive/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the adaptive manifest data shared with callers of this public contract.
 */
typedef struct UmiAdaptiveManifest {
    char application_id[UMI_ADAPTIVE_ID_CAPACITY];
    char shell_profile_id[UMI_ADAPTIVE_ID_CAPACITY];
    uint32_t renderer_mask;
    uint16_t breakpoint_count;
    int supports_orientation_change;
    int supports_multi_window;
} UmiAdaptiveManifest;
/* Initialise an application adaptive manifest. */
UmiStatus umi_adaptive_manifest_init(UmiAdaptiveManifest *manifest,
                                     const char *application_id,
                                     const char *shell_profile_id);
/* Validate minimum manifest identity and renderer coverage before launch. */
int umi_adaptive_manifest_valid(const UmiAdaptiveManifest *manifest);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_adaptive_manifest_archive_encode(const UmiAdaptiveManifest *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_adaptive_manifest_archive_decode(const void *bytes, size_t byte_count,
    UmiAdaptiveManifest *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiAdaptiveManifest object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_adaptive_manifest_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiAdaptiveManifest *workspace, UmiAdaptiveManifest *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_adaptive_manifest_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiAdaptiveManifest *expected, UmiAdaptiveManifest *value);


#ifdef __cplusplus
}
#endif
#endif
