/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/icon_descriptor.h
 *
 * PURPOSE:
 *   Describe a semantic icon identity, directionality and scalable/symbolic capabilities.
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
#ifndef UMICOM_UI_APPEARANCE_ICON_DESCRIPTOR_H
#define UMICOM_UI_APPEARANCE_ICON_DESCRIPTOR_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance icon descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceIconDescriptor {
    char icon_id[UMI_APPEARANCE_ID_CAPACITY];
    char semantic_role[UMI_APPEARANCE_ID_CAPACITY];
    bool scalable;
    bool symbolic;
    bool direction_sensitive;
} UmiAppearanceIconDescriptor;

/* Initialise one icon descriptor record with deterministic defaults. */
UmiStatus umi_appearance_icon_descriptor_init(UmiAppearanceIconDescriptor *item);
/* Validate the required production invariants for this icon descriptor. */
int umi_appearance_icon_descriptor_is_valid(const UmiAppearanceIconDescriptor *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_icon_descriptor_archive_encode(const UmiAppearanceIconDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_icon_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceIconDescriptor *value);

#ifdef __cplusplus
}
#endif
#endif
