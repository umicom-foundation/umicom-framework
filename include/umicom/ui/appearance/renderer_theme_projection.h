/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/renderer_theme_projection.h
 *
 * PURPOSE:
 *   Record one renderer-specific projection of a semantic style packet without transferring state ownership.
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
#ifndef UMICOM_UI_APPEARANCE_RENDERER_THEME_PROJECTION_H
#define UMICOM_UI_APPEARANCE_RENDERER_THEME_PROJECTION_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance renderer theme projection data shared with callers of this
 * public contract.
 */
typedef struct UmiAppearanceRendererThemeProjection {
    char projection_id[UMI_APPEARANCE_ID_CAPACITY];
    char packet_id[UMI_APPEARANCE_ID_CAPACITY];
    UmiAppearanceRendererKind renderer;
    uint64_t semantic_revision;
    uint64_t projected_revision;
    bool complete;
} UmiAppearanceRendererThemeProjection;

/* Initialise one renderer theme projection record with deterministic defaults. */
UmiStatus umi_appearance_renderer_theme_projection_init(UmiAppearanceRendererThemeProjection *item);
/* Validate the required production invariants for this renderer theme projection. */
int umi_appearance_renderer_theme_projection_is_valid(const UmiAppearanceRendererThemeProjection *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_renderer_theme_projection_archive_encode(const UmiAppearanceRendererThemeProjection *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_renderer_theme_projection_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceRendererThemeProjection *value);

#ifdef __cplusplus
}
#endif
#endif
