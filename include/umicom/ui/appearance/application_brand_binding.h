/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/application_brand_binding.h
 *
 * PURPOSE:
 *   Bind a thin application identity to Framework-owned brand and theme-pack identifiers.
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
#ifndef UMICOM_UI_APPEARANCE_APPLICATION_BRAND_BINDING_H
#define UMICOM_UI_APPEARANCE_APPLICATION_BRAND_BINDING_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance application brand binding data shared with callers of this
 * public contract.
 */
typedef struct UmiAppearanceApplicationBrandBinding {
    char application_id[UMI_APPEARANCE_ID_CAPACITY];
    char brand_id[UMI_APPEARANCE_ID_CAPACITY];
    char theme_pack_id[UMI_APPEARANCE_ID_CAPACITY];
} UmiAppearanceApplicationBrandBinding;

/* Initialise one application brand binding record with deterministic defaults. */
UmiStatus umi_appearance_application_brand_binding_init(UmiAppearanceApplicationBrandBinding *item);
/* Validate the required production invariants for this application brand binding. */
int umi_appearance_application_brand_binding_is_valid(const UmiAppearanceApplicationBrandBinding *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_application_brand_binding_archive_encode(const UmiAppearanceApplicationBrandBinding *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_application_brand_binding_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceApplicationBrandBinding *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_appearance_application_brand_binding_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiAppearanceApplicationBrandBinding *expected, UmiAppearanceApplicationBrandBinding *value);


#ifdef __cplusplus
}
#endif
#endif
