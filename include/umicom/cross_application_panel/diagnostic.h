/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/diagnostic.h
 *
 * PURPOSE:
 *   Define cross-application panel diagnostic state and bounded storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_DIAGNOSTIC_H
#define UMICOM_CROSS_APPLICATION_PANEL_DIAGNOSTIC_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPanelDiagnostic {
    uint32_t structure_size;
    char diagnostic_id[UMI_PANEL_TEXT_CAPACITY];
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char instance_id[UMI_PANEL_TEXT_CAPACITY];
    char message[UMI_PANEL_DESCRIPTION_CAPACITY];
    uint32_t severity;
    UmiStatus status;
    uint64_t revision;
} UmiPanelDiagnostic;
typedef struct UmiPanelDiagnosticStore { UmiPanelDiagnostic items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelDiagnosticStore;
void umi_panel_diagnostic_init(UmiPanelDiagnostic *record);
UmiStatus umi_panel_diagnostic_validate(const UmiPanelDiagnostic *record);
void umi_panel_diagnostic_store_init(UmiPanelDiagnosticStore *store);
UmiStatus umi_panel_diagnostic_store_put(UmiPanelDiagnosticStore *store,const UmiPanelDiagnostic *record);
UmiStatus umi_panel_diagnostic_store_remove(UmiPanelDiagnosticStore *store,const char *identity);
UmiPanelDiagnostic *umi_panel_diagnostic_store_find(UmiPanelDiagnosticStore *store,const char *identity);
const UmiPanelDiagnostic *umi_panel_diagnostic_store_find_const(const UmiPanelDiagnosticStore *store,const char *identity);
UmiStatus umi_panel_diagnostic_store_snapshot(const UmiPanelDiagnosticStore *store,UmiPanelDiagnostic *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
