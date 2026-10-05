/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/layout_restore_plan.h
 *
 * PURPOSE:
 *   Plan safe layout restoration when monitors or panels from a saved workspace are unavailable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_LAYOUT_RESTORE_PLAN_H
#define UMICOM_UI_MOSAIC_LAYOUT_RESTORE_PLAN_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic layout restore plan data shared with callers of this public
 * contract.
 */
typedef struct UmiUiMosaicLayoutRestorePlan {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t revision;
    uint32_t item_count;
    bool locked;
} UmiUiMosaicLayoutRestorePlan;

/* Initializes versioned layout state used by layout restore plan. */
void umi_ui_mosaic_layout_restore_plan_init(UmiUiMosaicLayoutRestorePlan *value);
/* Assigns the stable layout identity and display name. */
UmiStatus umi_ui_mosaic_layout_restore_plan_set(UmiUiMosaicLayoutRestorePlan *value, const char *id, const char *name);
/* Validates identity and bounded layout cardinality. */
UmiStatus umi_ui_mosaic_layout_restore_plan_validate(const UmiUiMosaicLayoutRestorePlan *value);
/* Advances the revision after a committed edit. */
UmiStatus umi_ui_mosaic_layout_restore_plan_touch(UmiUiMosaicLayoutRestorePlan *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_layout_restore_plan_archive_encode(const UmiUiMosaicLayoutRestorePlan *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_layout_restore_plan_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicLayoutRestorePlan *value);

#ifdef __cplusplus
}
#endif
#endif
