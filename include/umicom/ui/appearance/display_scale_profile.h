/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/display_scale_profile.h
 *
 * PURPOSE:
 *   Combine display DPI, operating-system scale and user accessibility scale into one profile.
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
#ifndef UMICOM_UI_APPEARANCE_DISPLAY_SCALE_PROFILE_H
#define UMICOM_UI_APPEARANCE_DISPLAY_SCALE_PROFILE_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance display scale profile data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceDisplayScaleProfile {
    char display_id[UMI_APPEARANCE_ID_CAPACITY];
    uint32_t dpi;
    double os_scale;
    double user_scale;
    double effective_scale;
} UmiAppearanceDisplayScaleProfile;

/* Initialise one display scale profile record with deterministic defaults. */
UmiStatus umi_appearance_display_scale_profile_init(UmiAppearanceDisplayScaleProfile *item);
/* Validate the required production invariants for this display scale profile. */
int umi_appearance_display_scale_profile_is_valid(const UmiAppearanceDisplayScaleProfile *item);
/* Recalculate the effective display scale after OS or user scale changes. */
UmiStatus umi_appearance_display_scale_profile_resolve(UmiAppearanceDisplayScaleProfile *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_display_scale_profile_archive_encode(const UmiAppearanceDisplayScaleProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_display_scale_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceDisplayScaleProfile *value);

#ifdef __cplusplus
}
#endif
#endif
