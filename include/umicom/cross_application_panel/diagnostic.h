/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/diagnostic.h
 *
 * PURPOSE:
 *   Define cross-application panel diagnostic state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_DIAGNOSTIC_H
#define UMICOM_CROSS_APPLICATION_PANEL_DIAGNOSTIC_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel diagnostic data shared with callers of this public contract.
 */
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
/**
 * Represent the panel diagnostic store data shared with callers of this public contract.
 */
typedef struct UmiPanelDiagnosticStore { UmiPanelDiagnostic items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelDiagnosticStore;
/**
 * Initialise panel diagnostic from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_diagnostic_init(UmiPanelDiagnostic *record);
/**
 * Check that panel diagnostic satisfies its contract before another service relies on it.
 */
UmiStatus umi_panel_diagnostic_validate(const UmiPanelDiagnostic *record);
/**
 * Initialise panel diagnostic store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_diagnostic_store_init(UmiPanelDiagnosticStore *store);
/**
 * Provide the panel diagnostic store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_diagnostic_store_put(UmiPanelDiagnosticStore *store,const UmiPanelDiagnostic *record);
/**
 * Remove panel diagnostic store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_diagnostic_store_remove(UmiPanelDiagnosticStore *store,const char *identity);
/**
 * Find panel diagnostic store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiPanelDiagnostic *umi_panel_diagnostic_store_find(UmiPanelDiagnosticStore *store,const char *identity);
/**
 * Provide the panel diagnostic store find const operation used by this module and its
 * client applications.
 */
const UmiPanelDiagnostic *umi_panel_diagnostic_store_find_const(const UmiPanelDiagnosticStore *store,const char *identity);
/**
 * Provide the panel diagnostic store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_diagnostic_store_snapshot(const UmiPanelDiagnosticStore *store,UmiPanelDiagnostic *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
