/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/tab_group.h
 *
 * PURPOSE:
 *   Define toolkit-neutral tab group contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_TAB_GROUP_H
#define UMICOM_UI_MOSAIC_TAB_GROUP_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic tab group data shared with callers of this public contract.
 */
typedef struct UmiUiMosaicTabGroup {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t revision;
    uint32_t item_count;
    bool locked;
} UmiUiMosaicTabGroup;

/* Initializes versioned layout state used by tab group. */
void umi_ui_mosaic_tab_group_init(UmiUiMosaicTabGroup *value);
/* Assigns the stable layout identity and display name. */
UmiStatus umi_ui_mosaic_tab_group_set(UmiUiMosaicTabGroup *value, const char *id, const char *name);
/* Validates identity and bounded layout cardinality. */
UmiStatus umi_ui_mosaic_tab_group_validate(const UmiUiMosaicTabGroup *value);
/* Advances the revision after a committed edit. */
UmiStatus umi_ui_mosaic_tab_group_touch(UmiUiMosaicTabGroup *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_tab_group_archive_encode(const UmiUiMosaicTabGroup *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_tab_group_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicTabGroup *value);

#ifdef __cplusplus
}
#endif
#endif
