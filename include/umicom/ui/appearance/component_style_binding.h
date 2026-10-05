/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/component_style_binding.h
 *
 * PURPOSE:
 *   Bind a semantic component and state map to a Framework style identity.
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
#ifndef UMICOM_UI_APPEARANCE_COMPONENT_STYLE_BINDING_H
#define UMICOM_UI_APPEARANCE_COMPONENT_STYLE_BINDING_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance component style binding data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceComponentStyleBinding {
    char component_id[UMI_APPEARANCE_ID_CAPACITY];
    char style_id[UMI_APPEARANCE_ID_CAPACITY];
    char state_map_id[UMI_APPEARANCE_ID_CAPACITY];
} UmiAppearanceComponentStyleBinding;

/* Initialise one component style binding record with deterministic defaults. */
UmiStatus umi_appearance_component_style_binding_init(UmiAppearanceComponentStyleBinding *item);
/* Validate the required production invariants for this component style binding. */
int umi_appearance_component_style_binding_is_valid(const UmiAppearanceComponentStyleBinding *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_component_style_binding_archive_encode(const UmiAppearanceComponentStyleBinding *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_component_style_binding_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceComponentStyleBinding *value);

#ifdef __cplusplus
}
#endif
#endif
