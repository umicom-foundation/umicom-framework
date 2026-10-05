/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/panel_activation.h
 *
 * PURPOSE:
 *   Define toolkit-neutral panel activation contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_PANEL_ACTIVATION_H
#define UMICOM_UI_MOSAIC_PANEL_ACTIVATION_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic panel activation data shared with callers of this public
 * contract.
 */
typedef struct UmiUiMosaicPanelActivation {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char title[UMI_UI_MOSAIC_TEXT_CAPACITY];
    UmiUiMosaicApplication application;
    uint32_t priority;
    bool enabled;
} UmiUiMosaicPanelActivation;

/* Initializes a bounded panel activation record with safe defaults. */
void umi_ui_mosaic_panel_activation_init(UmiUiMosaicPanelActivation *value);
/* Assigns the stable identifier and user-visible title. */
UmiStatus umi_ui_mosaic_panel_activation_set(UmiUiMosaicPanelActivation *value, const char *id, const char *title);
/* Verifies identifiers, application ownership and enabled state. */
UmiStatus umi_ui_mosaic_panel_activation_validate(const UmiUiMosaicPanelActivation *value);
/* Produces a deterministic ranking key used by catalogue/search surfaces. */
uint32_t umi_ui_mosaic_panel_activation_rank(const UmiUiMosaicPanelActivation *value, uint32_t context_boost);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_panel_activation_archive_encode(const UmiUiMosaicPanelActivation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_panel_activation_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicPanelActivation *value);

#ifdef __cplusplus
}
#endif
#endif
