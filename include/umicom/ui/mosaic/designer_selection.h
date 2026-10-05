/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/designer_selection.h
 *
 * PURPOSE:
 *   Define toolkit-neutral designer selection contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_DESIGNER_SELECTION_H
#define UMICOM_UI_MOSAIC_DESIGNER_SELECTION_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic designer selection data shared with callers of this public
 * contract.
 */
typedef struct UmiUiMosaicDesignerSelection {
    char workspace_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char active_id[UMI_UI_MOSAIC_ID_CAPACITY];
    uint32_t revision;
    uint32_t selection_count;
    UmiUiMosaicEditMode mode;
    bool valid;
} UmiUiMosaicDesignerSelection;

/* Initializes renderer-neutral Layout Designer state. */
void umi_ui_mosaic_designer_selection_init(UmiUiMosaicDesignerSelection *value);
/* Binds the designer state to a workspace and active object. */
UmiStatus umi_ui_mosaic_designer_selection_bind(UmiUiMosaicDesignerSelection *value, const char *workspace_id, const char *active_id);
/* Validates selection/edit state before a layout mutation is committed. */
UmiStatus umi_ui_mosaic_designer_selection_validate(const UmiUiMosaicDesignerSelection *value);
/* Advances the designer revision after one accepted operation. */
UmiStatus umi_ui_mosaic_designer_selection_advance(UmiUiMosaicDesignerSelection *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_designer_selection_archive_encode(const UmiUiMosaicDesignerSelection *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_designer_selection_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicDesignerSelection *value);

#ifdef __cplusplus
}
#endif
#endif
