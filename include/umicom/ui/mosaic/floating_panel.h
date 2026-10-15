/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/floating_panel.h
 *
 * PURPOSE:
 *   Define toolkit-neutral floating panel contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_FLOATING_PANEL_H
#define UMICOM_UI_MOSAIC_FLOATING_PANEL_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic floating panel data shared with callers of this public contract.
 */
typedef struct UmiUiMosaicFloatingPanel {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t revision;
    uint32_t item_count;
    bool locked;
} UmiUiMosaicFloatingPanel;

/* Initializes versioned layout state used by floating panel. */
void umi_ui_mosaic_floating_panel_init(UmiUiMosaicFloatingPanel *value);
/* Assigns the stable layout identity and display name. */
UmiStatus umi_ui_mosaic_floating_panel_set(UmiUiMosaicFloatingPanel *value, const char *id, const char *name);
/* Validates identity and bounded layout cardinality. */
UmiStatus umi_ui_mosaic_floating_panel_validate(const UmiUiMosaicFloatingPanel *value);
/* Advances the revision after a committed edit. */
UmiStatus umi_ui_mosaic_floating_panel_touch(UmiUiMosaicFloatingPanel *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_floating_panel_archive_encode(const UmiUiMosaicFloatingPanel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_floating_panel_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicFloatingPanel *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ui_mosaic_floating_panel_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiUiMosaicFloatingPanel *expected, UmiUiMosaicFloatingPanel *value);


#ifdef __cplusplus
}
#endif
#endif
