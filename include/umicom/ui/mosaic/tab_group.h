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
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
#endif
