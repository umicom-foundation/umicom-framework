/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/preview_state.h
 *
 * PURPOSE:
 *   Record renderer-neutral preview health and diagnostic counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_PREVIEW_STATE_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_PREVIEW_STATE_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer preview state data shared with callers of this public contract.
 */
typedef struct UmiRadPreviewState {
    uint64_t document_revision;
    uint64_t render_revision;
    size_t warning_count;
    size_t error_count;
    bool healthy;
} UmiRadPreviewState;
/**
 * Initialise visual designer preview state from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_preview_state_init(UmiRadPreviewState *item);
/**
 * Check that visual designer preview state satisfies its contract before another service relies on it.
 */
int umi_rad_preview_state_is_valid(const UmiRadPreviewState *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_preview_state_archive_encode(const UmiRadPreviewState *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_preview_state_archive_decode(const void *bytes, size_t byte_count,
    UmiRadPreviewState *value);

#ifdef __cplusplus
}
#endif
#endif
