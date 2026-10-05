/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/theme_resolution.h
 *
 * PURPOSE:
 *   Record deterministic system/application/workspace/component theme resolution evidence.
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
#ifndef UMICOM_UI_APPEARANCE_THEME_RESOLUTION_H
#define UMICOM_UI_APPEARANCE_THEME_RESOLUTION_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance theme resolution data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceThemeResolution {
    char requested_pack_id[UMI_APPEARANCE_ID_CAPACITY];
    char resolved_pack_id[UMI_APPEARANCE_ID_CAPACITY];
    UmiAppearanceScope winning_scope;
    uint32_t inherited_layers;
} UmiAppearanceThemeResolution;

/* Initialise one theme resolution record with deterministic defaults. */
UmiStatus umi_appearance_theme_resolution_init(UmiAppearanceThemeResolution *item);
/* Validate the required production invariants for this theme resolution. */
int umi_appearance_theme_resolution_is_valid(const UmiAppearanceThemeResolution *item);
/* Resolve precedence from component through workspace/application to system. */
UmiStatus umi_appearance_theme_resolution_choose(UmiAppearanceThemeResolution *item, const char *system_id, const char *application_id, const char *workspace_id, const char *component_id);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_theme_resolution_archive_encode(const UmiAppearanceThemeResolution *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_theme_resolution_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceThemeResolution *value);

#ifdef __cplusplus
}
#endif
#endif
