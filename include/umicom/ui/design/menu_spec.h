/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/menu_spec.h
 *
 * PURPOSE:
 *   Define scalable menu presentation, search and overflow semantics.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_MENU_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_MENU_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/base/value_archive.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the design menu spec data shared with callers of this public contract.
 */
typedef struct UmiDesignMenuSpec {
    uint16_t max_visible_items;
    int searchable;
    int icons;
    int accelerators;
} UmiDesignMenuSpec;

/* Initialise the semantic menu spec specification. */
UmiStatus umi_design_menu_spec_init(UmiDesignMenuSpec *spec, uint16_t max_visible_items, int searchable, int icons, int accelerators);
/* Return one when the semantic specification is internally consistent. */
int umi_design_menu_spec_valid(const UmiDesignMenuSpec *spec);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_design_menu_spec_archive_encode(const UmiDesignMenuSpec *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_design_menu_spec_archive_decode(const void *bytes, size_t byte_count,
    UmiDesignMenuSpec *value);

#ifdef __cplusplus
}
#endif

#endif
