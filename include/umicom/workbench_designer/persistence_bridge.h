/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/persistence_bridge.h
 *
 * PURPOSE:
 *   Connect designer save, load, list, delete and session checkpoint requests to
 *   the authoritative layout persistence port.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_PERSISTENCE_BRIDGE_H
#define UMICOM_WORKBENCH_DESIGNER_PERSISTENCE_BRIDGE_H

#include "umicom/workbench_designer/types.h"
#include "umicom/workbench_layout/store.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerPersistenceBridge {
    UmiWorkbenchLayoutStoreAdapter adapter;
    UmiWorkbenchDesignerSaveState save_state;
    uint64_t persisted_revision;
    uint64_t last_save_started_ms;
    uint64_t last_save_completed_ms;
    UmiStatus last_status;
    char last_error[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    uint64_t revision;
} UmiWorkbenchDesignerPersistenceBridge;

UmiStatus umi_workbench_designer_persistence_bridge_init(UmiWorkbenchDesignerPersistenceBridge *bridge, const UmiWorkbenchLayoutStoreAdapter *adapter);
void umi_workbench_designer_persistence_bridge_mark_dirty(UmiWorkbenchDesignerPersistenceBridge *bridge);
UmiStatus umi_workbench_designer_persistence_bridge_save(UmiWorkbenchDesignerPersistenceBridge *bridge, UmiWorkbenchLayoutDocument *document, uint64_t expected_revision, uint64_t timestamp_ms);
UmiStatus umi_workbench_designer_persistence_bridge_load(UmiWorkbenchDesignerPersistenceBridge *bridge, const char *layout_id, UmiWorkbenchLayoutDocument *out_document);
UmiStatus umi_workbench_designer_persistence_bridge_delete(UmiWorkbenchDesignerPersistenceBridge *bridge, const char *layout_id, uint64_t expected_revision);
UmiStatus umi_workbench_designer_persistence_bridge_list(UmiWorkbenchDesignerPersistenceBridge *bridge, const UmiWorkbenchLayoutStoreQuery *query, UmiWorkbenchLayoutStoreList *out_list);

#ifdef __cplusplus
}
#endif

#endif
