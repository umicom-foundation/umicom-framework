/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/command.h
 *
 * PURPOSE:
 *   Define cross-application panel command state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_COMMAND_H
#define UMICOM_CROSS_APPLICATION_PANEL_COMMAND_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPanelCommand {
    uint32_t structure_size;
    char command_id[UMI_PANEL_TEXT_CAPACITY];
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char instance_id[UMI_PANEL_TEXT_CAPACITY];
    char action_id[UMI_PANEL_TEXT_CAPACITY];
    char argument[UMI_PANEL_DESCRIPTION_CAPACITY];
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiPanelCommand;
typedef struct UmiPanelCommandStore { UmiPanelCommand items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelCommandStore;
void umi_panel_command_init(UmiPanelCommand *record);
UmiStatus umi_panel_command_validate(const UmiPanelCommand *record);
void umi_panel_command_store_init(UmiPanelCommandStore *store);
UmiStatus umi_panel_command_store_put(UmiPanelCommandStore *store,const UmiPanelCommand *record);
UmiStatus umi_panel_command_store_remove(UmiPanelCommandStore *store,const char *identity);
UmiPanelCommand *umi_panel_command_store_find(UmiPanelCommandStore *store,const char *identity);
const UmiPanelCommand *umi_panel_command_store_find_const(const UmiPanelCommandStore *store,const char *identity);
UmiStatus umi_panel_command_store_snapshot(const UmiPanelCommandStore *store,UmiPanelCommand *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
