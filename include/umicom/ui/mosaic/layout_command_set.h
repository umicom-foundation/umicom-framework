/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/layout_command_set.h
 *
 * PURPOSE:
 *   Define toolkit-neutral layout command set contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_LAYOUT_COMMAND_SET_H
#define UMICOM_UI_MOSAIC_LAYOUT_COMMAND_SET_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic layout command set data shared with callers of this public
 * contract.
 */
typedef struct UmiUiMosaicLayoutCommandSet {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char label[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t ordinal;
    bool enabled;
    bool requires_edit_mode;
} UmiUiMosaicLayoutCommandSet;

/* Initializes a stable Layout Designer command descriptor. */
void umi_ui_mosaic_layout_command_set_init(UmiUiMosaicLayoutCommandSet *value);
/* Assigns command identity and display label. */
UmiStatus umi_ui_mosaic_layout_command_set_set(UmiUiMosaicLayoutCommandSet *value, const char *id, const char *label);
/* Validates command identity before exposing it to menus/toolbars. */
UmiStatus umi_ui_mosaic_layout_command_set_validate(const UmiUiMosaicLayoutCommandSet *value);
/* Evaluates whether the command can run in the current edit mode. */
int umi_ui_mosaic_layout_command_set_can_execute(const UmiUiMosaicLayoutCommandSet *value, UmiUiMosaicEditMode mode);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_layout_command_set_archive_encode(const UmiUiMosaicLayoutCommandSet *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_layout_command_set_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicLayoutCommandSet *value);

#ifdef __cplusplus
}
#endif
#endif
