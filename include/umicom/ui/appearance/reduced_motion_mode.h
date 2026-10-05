/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/reduced_motion_mode.h
 *
 * PURPOSE:
 *   Resolve reduced-motion presentation requirements from user and system accessibility settings.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_REDUCED_MOTION_MODE_H
#define UMICOM_UI_APPEARANCE_REDUCED_MOTION_MODE_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance reduced motion mode data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceReducedMotionMode {
    char mode_id[UMI_APPEARANCE_ID_CAPACITY];
    bool enabled;
    uint32_t maximum_duration_ms;
    bool disable_decorative;
    bool preserve_essential_feedback;
} UmiAppearanceReducedMotionMode;

/* Initialise one reduced motion mode record with deterministic defaults. */
UmiStatus umi_appearance_reduced_motion_mode_init(UmiAppearanceReducedMotionMode *item);
/* Validate the required production invariants for this reduced motion mode. */
int umi_appearance_reduced_motion_mode_is_valid(const UmiAppearanceReducedMotionMode *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_reduced_motion_mode_archive_encode(const UmiAppearanceReducedMotionMode *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_reduced_motion_mode_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceReducedMotionMode *value);

#ifdef __cplusplus
}
#endif
#endif
