/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/theme_variant.h
 *
 * PURPOSE:
 *   Bind a semantic theme pack to light, dark or high-contrast presentation mode.
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
#ifndef UMICOM_UI_APPEARANCE_THEME_VARIANT_H
#define UMICOM_UI_APPEARANCE_THEME_VARIANT_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance theme variant data shared with callers of this public contract.
 */
typedef struct UmiAppearanceThemeVariant {
    char variant_id[UMI_APPEARANCE_ID_CAPACITY];
    char pack_id[UMI_APPEARANCE_ID_CAPACITY];
    UmiDesignThemeMode mode;
    bool preferred;
} UmiAppearanceThemeVariant;

/* Initialise one theme variant record with deterministic defaults. */
UmiStatus umi_appearance_theme_variant_init(UmiAppearanceThemeVariant *item);
/* Validate the required production invariants for this theme variant. */
int umi_appearance_theme_variant_is_valid(const UmiAppearanceThemeVariant *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_theme_variant_archive_encode(const UmiAppearanceThemeVariant *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_theme_variant_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceThemeVariant *value);

#ifdef __cplusplus
}
#endif
#endif
