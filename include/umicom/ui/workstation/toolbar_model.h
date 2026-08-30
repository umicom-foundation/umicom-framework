/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/toolbar_model.h
 *
 * PURPOSE:
 *   Define toolkit-neutral professional toolbar items shared by IDE, trading, media and design workstations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_TOOLBAR_MODEL_H
#define UMICOM_UI_WORKSTATION_TOOLBAR_MODEL_H

#include <stdbool.h>
#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiWsToolbarItemKind {
    UMI_WS_TOOLBAR_BUTTON = 1,
    UMI_WS_TOOLBAR_TOGGLE = 2,
    UMI_WS_TOOLBAR_SELECT = 3,
    UMI_WS_TOOLBAR_SEPARATOR = 4,
    UMI_WS_TOOLBAR_SPACER = 5
} UmiWsToolbarItemKind;

typedef struct UmiWsToolbarItem {
    char item_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char command_id[UMI_UI_ID_CAPACITY];
    UmiWsToolbarItemKind kind;
    bool enabled;
    bool active;
} UmiWsToolbarItem;

typedef struct UmiWsToolbarModel {
    UmiWsToolbarItem items[UMI_WS_MAX_TOOLBAR_ITEMS];
    size_t count;
} UmiWsToolbarModel;

void umi_ws_toolbar_model_init(UmiWsToolbarModel *toolbar);
UmiStatus umi_ws_toolbar_model_add(UmiWsToolbarModel *toolbar,
                                   const char *item_id,
                                   const char *label,
                                   const char *command_id,
                                   UmiWsToolbarItemKind kind);
UmiStatus umi_ws_toolbar_model_set_active(UmiWsToolbarModel *toolbar, const char *item_id, bool active);

#ifdef __cplusplus
}
#endif

#endif
