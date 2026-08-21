/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/factory_record.h
 *
 * PURPOSE:
 *   Define cross-application panel factory record state and bounded storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_FACTORY_RECORD_H
#define UMICOM_CROSS_APPLICATION_PANEL_FACTORY_RECORD_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPanelFactoryRecord {
    uint32_t structure_size;
    char factory_id[UMI_PANEL_TEXT_CAPACITY];
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char provider_id[UMI_PANEL_TEXT_CAPACITY];
    char component_id[UMI_PANEL_TEXT_CAPACITY];
    bool enabled;
    uint32_t priority;
    uint64_t revision;
} UmiPanelFactoryRecord;
typedef struct UmiPanelFactoryRecordStore { UmiPanelFactoryRecord items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelFactoryRecordStore;
void umi_panel_factory_record_init(UmiPanelFactoryRecord *record);
UmiStatus umi_panel_factory_record_validate(const UmiPanelFactoryRecord *record);
void umi_panel_factory_record_store_init(UmiPanelFactoryRecordStore *store);
UmiStatus umi_panel_factory_record_store_put(UmiPanelFactoryRecordStore *store,const UmiPanelFactoryRecord *record);
UmiStatus umi_panel_factory_record_store_remove(UmiPanelFactoryRecordStore *store,const char *identity);
UmiPanelFactoryRecord *umi_panel_factory_record_store_find(UmiPanelFactoryRecordStore *store,const char *identity);
const UmiPanelFactoryRecord *umi_panel_factory_record_store_find_const(const UmiPanelFactoryRecordStore *store,const char *identity);
UmiStatus umi_panel_factory_record_store_snapshot(const UmiPanelFactoryRecordStore *store,UmiPanelFactoryRecord *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
