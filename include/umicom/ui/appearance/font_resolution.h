/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/font_resolution.h
 *
 * PURPOSE:
 *   Record the winning family and fallback depth selected for a semantic font stack.
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
#ifndef UMICOM_UI_APPEARANCE_FONT_RESOLUTION_H
#define UMICOM_UI_APPEARANCE_FONT_RESOLUTION_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance font resolution data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceFontResolution {
    char stack_id[UMI_APPEARANCE_ID_CAPACITY];
    char resolved_family_id[UMI_APPEARANCE_ID_CAPACITY];
    uint32_t fallback_depth;
    bool available;
} UmiAppearanceFontResolution;

/* Initialise one font resolution record with deterministic defaults. */
UmiStatus umi_appearance_font_resolution_init(UmiAppearanceFontResolution *item);
/* Validate the required production invariants for this font resolution. */
int umi_appearance_font_resolution_is_valid(const UmiAppearanceFontResolution *item);
/* Resolve to the preferred available family, falling back when the preferred identity is empty. */
UmiStatus umi_appearance_font_resolution_choose(UmiAppearanceFontResolution *item,const char *preferred,const char *fallback);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_font_resolution_archive_encode(const UmiAppearanceFontResolution *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_font_resolution_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceFontResolution *value);

#ifdef __cplusplus
}
#endif
#endif
