/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/dock_history.h
 *
 * PURPOSE:
 *   Define toolkit-neutral dock history contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_DOCK_HISTORY_H
#define UMICOM_UI_MOSAIC_DOCK_HISTORY_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic dock history data shared with callers of this public contract.
 */
typedef struct UmiUiMosaicDockHistory {
    char source_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char target_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicDockZone zone;
    uint32_t sequence;
    bool allowed;
} UmiUiMosaicDockHistory;

/* Initializes one dock history relation. */
void umi_ui_mosaic_dock_history_init(UmiUiMosaicDockHistory *value);
/* Configures source/target/zone without performing a renderer mutation. */
UmiStatus umi_ui_mosaic_dock_history_set(UmiUiMosaicDockHistory *value, const char *source_id, const char *target_id, UmiUiMosaicDockZone zone);
/* Rejects malformed or self-referential dock operations. */
UmiStatus umi_ui_mosaic_dock_history_validate(const UmiUiMosaicDockHistory *value);
/* Returns whether the relation is a centre/tab-style target. */
int umi_ui_mosaic_dock_history_is_centre(const UmiUiMosaicDockHistory *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_dock_history_archive_encode(const UmiUiMosaicDockHistory *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_dock_history_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicDockHistory *value);

#ifdef __cplusplus
}
#endif
#endif
