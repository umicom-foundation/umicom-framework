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

#ifdef __cplusplus
}
#endif
#endif
