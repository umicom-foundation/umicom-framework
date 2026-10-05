/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/panel_query.h
 *
 * PURPOSE:
 *   Define toolkit-neutral panel query contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_PANEL_QUERY_H
#define UMICOM_UI_MOSAIC_PANEL_QUERY_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic panel query data shared with callers of this public contract.
 */
typedef struct UmiUiMosaicPanelQuery {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char title[UMI_UI_MOSAIC_TEXT_CAPACITY];
    UmiUiMosaicApplication application;
    uint32_t priority;
    bool enabled;
} UmiUiMosaicPanelQuery;

/* Initializes a bounded panel query record with safe defaults. */
void umi_ui_mosaic_panel_query_init(UmiUiMosaicPanelQuery *value);
/* Assigns the stable identifier and user-visible title. */
UmiStatus umi_ui_mosaic_panel_query_set(UmiUiMosaicPanelQuery *value, const char *id, const char *title);
/* Verifies identifiers, application ownership and enabled state. */
UmiStatus umi_ui_mosaic_panel_query_validate(const UmiUiMosaicPanelQuery *value);
/* Produces a deterministic ranking key used by catalogue/search surfaces. */
uint32_t umi_ui_mosaic_panel_query_rank(const UmiUiMosaicPanelQuery *value, uint32_t context_boost);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_panel_query_archive_encode(const UmiUiMosaicPanelQuery *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_panel_query_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicPanelQuery *value);

#ifdef __cplusplus
}
#endif
#endif
