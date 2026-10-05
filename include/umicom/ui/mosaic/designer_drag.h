/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/designer_drag.h
 *
 * PURPOSE:
 *   Define toolkit-neutral designer drag contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_DESIGNER_DRAG_H
#define UMICOM_UI_MOSAIC_DESIGNER_DRAG_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic designer drag data shared with callers of this public contract.
 */
typedef struct UmiUiMosaicDesignerDrag {
    char workspace_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char active_id[UMI_UI_MOSAIC_ID_CAPACITY];
    uint32_t revision;
    uint32_t selection_count;
    UmiUiMosaicEditMode mode;
    bool valid;
} UmiUiMosaicDesignerDrag;

/* Initializes renderer-neutral Layout Designer state. */
void umi_ui_mosaic_designer_drag_init(UmiUiMosaicDesignerDrag *value);
/* Binds the designer state to a workspace and active object. */
UmiStatus umi_ui_mosaic_designer_drag_bind(UmiUiMosaicDesignerDrag *value, const char *workspace_id, const char *active_id);
/* Validates selection/edit state before a layout mutation is committed. */
UmiStatus umi_ui_mosaic_designer_drag_validate(const UmiUiMosaicDesignerDrag *value);
/* Advances the designer revision after one accepted operation. */
UmiStatus umi_ui_mosaic_designer_drag_advance(UmiUiMosaicDesignerDrag *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_designer_drag_archive_encode(const UmiUiMosaicDesignerDrag *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_designer_drag_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicDesignerDrag *value);

#ifdef __cplusplus
}
#endif
#endif
