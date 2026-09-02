/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/persistence_bridge.h
 *
 * PURPOSE:
 *   Connect designer save, load, list, delete and session checkpoint requests to
 *   the authoritative layout persistence port.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_PERSISTENCE_BRIDGE_H
#define UMICOM_WORKBENCH_DESIGNER_PERSISTENCE_BRIDGE_H

#include "umicom/workbench_designer/types.h"
#include "umicom/workbench_layout/store.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer persistence bridge data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise workbench designer persistence bridge from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_workbench_designer_persistence_bridge_init(UmiWorkbenchDesignerPersistenceBridge *bridge, const UmiWorkbenchLayoutStoreAdapter *adapter);
/**
 * Provide the workbench designer persistence bridge mark dirty operation used by this
 * module and its client applications.
 */
void umi_workbench_designer_persistence_bridge_mark_dirty(UmiWorkbenchDesignerPersistenceBridge *bridge);
/**
 * Write workbench designer persistence bridge in its stable representation and report
 * capacity or input failures to the caller.
 */
UmiStatus umi_workbench_designer_persistence_bridge_save(UmiWorkbenchDesignerPersistenceBridge *bridge, UmiWorkbenchLayoutDocument *document, uint64_t expected_revision, uint64_t timestamp_ms);
/**
 * Read workbench designer persistence bridge into validated module state and return a
 * status when input cannot be used.
 */
UmiStatus umi_workbench_designer_persistence_bridge_load(UmiWorkbenchDesignerPersistenceBridge *bridge, const char *layout_id, UmiWorkbenchLayoutDocument *out_document);
/**
 * Provide the workbench designer persistence bridge delete operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_designer_persistence_bridge_delete(UmiWorkbenchDesignerPersistenceBridge *bridge, const char *layout_id, uint64_t expected_revision);
/**
 * Provide the workbench designer persistence bridge list operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_persistence_bridge_list(UmiWorkbenchDesignerPersistenceBridge *bridge, const UmiWorkbenchLayoutStoreQuery *query, UmiWorkbenchLayoutStoreList *out_list);

#ifdef __cplusplus
}
#endif

#endif
