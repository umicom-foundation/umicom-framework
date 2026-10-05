/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/renderer_appearance_capability.h
 *
 * PURPOSE:
 *   Declare appearance capabilities and limitations for GTK4, Qt6, Native Web or headless renderers.
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
#ifndef UMICOM_UI_APPEARANCE_RENDERER_APPEARANCE_CAPABILITY_H
#define UMICOM_UI_APPEARANCE_RENDERER_APPEARANCE_CAPABILITY_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance renderer appearance capability data shared with callers of this
 * public contract.
 */
typedef struct UmiAppearanceRendererAppearanceCapability {
    char renderer_id[UMI_APPEARANCE_ID_CAPACITY];
    UmiAppearanceRendererKind kind;
    bool supports_fractional_scale;
    bool supports_high_contrast;
    bool supports_reduced_motion;
    bool supports_symbolic_icons;
} UmiAppearanceRendererAppearanceCapability;

/* Initialise one renderer appearance capability record with deterministic defaults. */
UmiStatus umi_appearance_renderer_appearance_capability_init(UmiAppearanceRendererAppearanceCapability *item);
/* Validate the required production invariants for this renderer appearance capability. */
int umi_appearance_renderer_appearance_capability_is_valid(const UmiAppearanceRendererAppearanceCapability *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_renderer_appearance_capability_archive_encode(const UmiAppearanceRendererAppearanceCapability *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_renderer_appearance_capability_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceRendererAppearanceCapability *value);

#ifdef __cplusplus
}
#endif
#endif
