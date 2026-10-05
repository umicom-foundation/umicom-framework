/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/line_height_policy.h
 *
 * PURPOSE:
 *   Maintain readable line-height bounds as font and accessibility scale changes.
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
#ifndef UMICOM_UI_APPEARANCE_LINE_HEIGHT_POLICY_H
#define UMICOM_UI_APPEARANCE_LINE_HEIGHT_POLICY_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance line height policy data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceLineHeightPolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    double minimum_multiplier;
    double preferred_multiplier;
    double maximum_multiplier;
} UmiAppearanceLineHeightPolicy;

/* Initialise one line height policy record with deterministic defaults. */
UmiStatus umi_appearance_line_height_policy_init(UmiAppearanceLineHeightPolicy *item);
/* Validate the required production invariants for this line height policy. */
int umi_appearance_line_height_policy_is_valid(const UmiAppearanceLineHeightPolicy *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_line_height_policy_archive_encode(const UmiAppearanceLineHeightPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_line_height_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceLineHeightPolicy *value);

#ifdef __cplusplus
}
#endif
#endif
