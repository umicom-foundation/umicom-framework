/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/appearance_profile.h
 *
 * PURPOSE:
 *   Capture a resolved user/application appearance profile shared by every renderer.
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
#ifndef UMICOM_UI_APPEARANCE_APPEARANCE_PROFILE_H
#define UMICOM_UI_APPEARANCE_APPEARANCE_PROFILE_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance appearance profile data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceAppearanceProfile {
    char profile_id[UMI_APPEARANCE_ID_CAPACITY];
    UmiDesignThemeMode theme_mode;
    UmiDesignDensity density;
    double text_scale;
    double display_scale;
    bool reduced_motion;
    bool high_contrast;
} UmiAppearanceAppearanceProfile;

/* Initialise one appearance profile record with deterministic defaults. */
UmiStatus umi_appearance_profile_init(UmiAppearanceAppearanceProfile *item);
/* Validate the required production invariants for this appearance profile. */
int umi_appearance_profile_is_valid(const UmiAppearanceAppearanceProfile *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_profile_archive_encode(const UmiAppearanceAppearanceProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceAppearanceProfile *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_appearance_profile_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiAppearanceAppearanceProfile *expected, UmiAppearanceAppearanceProfile *value);


#ifdef __cplusplus
}
#endif
#endif
