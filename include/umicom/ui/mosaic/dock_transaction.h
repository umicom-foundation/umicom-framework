/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/dock_transaction.h
 *
 * PURPOSE:
 *   Represent reviewable dock mutations so layout edits can be validated, committed or cancelled atomically.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_DOCK_TRANSACTION_H
#define UMICOM_UI_MOSAIC_DOCK_TRANSACTION_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicDockTransaction {
    char source_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char target_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicDockZone zone;
    uint32_t sequence;
    bool allowed;
} UmiUiMosaicDockTransaction;

/* Initializes one dock transaction relation. */
void umi_ui_mosaic_dock_transaction_init(UmiUiMosaicDockTransaction *value);
/* Configures source/target/zone without performing a renderer mutation. */
UmiStatus umi_ui_mosaic_dock_transaction_set(UmiUiMosaicDockTransaction *value, const char *source_id, const char *target_id, UmiUiMosaicDockZone zone);
/* Rejects malformed or self-referential dock operations. */
UmiStatus umi_ui_mosaic_dock_transaction_validate(const UmiUiMosaicDockTransaction *value);
/* Returns whether the relation is a centre/tab-style target. */
int umi_ui_mosaic_dock_transaction_is_centre(const UmiUiMosaicDockTransaction *value);

#ifdef __cplusplus
}
#endif
#endif
