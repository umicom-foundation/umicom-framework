/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/typography_policy.h
 *
 * PURPOSE:
 *   Govern semantic typography scaling, minimum readable text size and font smoothing intent.
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
#ifndef UMICOM_UI_APPEARANCE_TYPOGRAPHY_POLICY_H
#define UMICOM_UI_APPEARANCE_TYPOGRAPHY_POLICY_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance typography policy data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceTypographyPolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    double base_text_scale;
    double minimum_text_dp;
    double maximum_text_scale;
    bool respect_user_scale;
} UmiAppearanceTypographyPolicy;

/* Initialise one typography policy record with deterministic defaults. */
UmiStatus umi_appearance_typography_policy_init(UmiAppearanceTypographyPolicy *item);
/* Validate the required production invariants for this typography policy. */
int umi_appearance_typography_policy_is_valid(const UmiAppearanceTypographyPolicy *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_typography_policy_archive_encode(const UmiAppearanceTypographyPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_typography_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceTypographyPolicy *value);

#ifdef __cplusplus
}
#endif
#endif
