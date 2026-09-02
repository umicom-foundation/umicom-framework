/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/persistence_bridge.c
 *
 * PURPOSE:
 *   Apply optimistic revisions and explicit save-state transitions around the
 *   existing layout store adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/persistence_bridge.h"
#include "internal.h"


/*
 * Initialise workbench designer persistence bridge from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_workbench_designer_persistence_bridge_init(
    UmiWorkbenchDesignerPersistenceBridge *bridge,
    const UmiWorkbenchLayoutStoreAdapter *adapter)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL || adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_layout_store_adapter_validate(adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(bridge, 0, sizeof(*bridge));
    bridge->adapter = *adapter;
    bridge->save_state = UMI_WORKBENCH_DESIGNER_SAVE_CLEAN;
    bridge->last_status = UMI_STATUS_OK;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer persistence bridge mark dirty operation used by this
 * module and its client applications.
 */
void umi_workbench_designer_persistence_bridge_mark_dirty(
    UmiWorkbenchDesignerPersistenceBridge *bridge)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (bridge->save_state != UMI_WORKBENCH_DESIGNER_SAVE_SAVING) {
        bridge->save_state = UMI_WORKBENCH_DESIGNER_SAVE_DIRTY;
    }
    bridge->revision += 1U;
}

/*
 * Write workbench designer persistence bridge in its stable representation and report
 * capacity or input failures to the caller.
 */
UmiStatus umi_workbench_designer_persistence_bridge_save(
    UmiWorkbenchDesignerPersistenceBridge *bridge,
    UmiWorkbenchLayoutDocument *document,
    uint64_t expected_revision,
    uint64_t timestamp_ms)
{
    uint64_t resulting_revision = 0U;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL || document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    bridge->save_state = UMI_WORKBENCH_DESIGNER_SAVE_SAVING;
    bridge->last_save_started_ms = timestamp_ms;
    bridge->last_error[0] = '\0';
    bridge->revision += 1U;
    status = umi_workbench_layout_store_save(
        &bridge->adapter, document, expected_revision, &resulting_revision);
    bridge->last_status = status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        bridge->persisted_revision = resulting_revision;
        bridge->last_save_completed_ms = timestamp_ms;
        bridge->save_state = UMI_WORKBENCH_DESIGNER_SAVE_CLEAN;
        document->version.revision = resulting_revision;
        document->version.base_revision = resulting_revision;
        (void)umi_workbench_layout_document_set_flag(
            document, UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY, false);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        bridge->save_state = status == UMI_STATUS_INVALID_STATE
            ? UMI_WORKBENCH_DESIGNER_SAVE_CONFLICT
            : UMI_WORKBENCH_DESIGNER_SAVE_FAILED;
        (void)umi_workbench_designer_copy_text(
            bridge->last_error, sizeof(bridge->last_error),
            status == UMI_STATUS_INVALID_STATE
                ? "The stored layout has a newer revision."
                : "The layout store rejected the save request.");
    }
    bridge->revision += 1U;
    return status;
}

/*
 * Read workbench designer persistence bridge into validated module state and return a
 * status when input cannot be used.
 */
UmiStatus umi_workbench_designer_persistence_bridge_load(
    UmiWorkbenchDesignerPersistenceBridge *bridge,
    const char *layout_id,
    UmiWorkbenchLayoutDocument *out_document)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL || layout_id == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_store_load(
        &bridge->adapter, layout_id, out_document);
    bridge->last_status = status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        bridge->persisted_revision = out_document->version.revision;
        bridge->save_state = UMI_WORKBENCH_DESIGNER_SAVE_CLEAN;
        bridge->last_error[0] = '\0';
    }
    bridge->revision += 1U;
    return status;
}

/*
 * Provide the workbench designer persistence bridge delete operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_designer_persistence_bridge_delete(
    UmiWorkbenchDesignerPersistenceBridge *bridge,
    const char *layout_id,
    uint64_t expected_revision)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL || layout_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_layout_store_delete(
        &bridge->adapter, layout_id, expected_revision);
    bridge->last_status = status;
    bridge->revision += 1U;
    return status;
}

/*
 * Provide the workbench designer persistence bridge list operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_persistence_bridge_list(
    UmiWorkbenchDesignerPersistenceBridge *bridge,
    const UmiWorkbenchLayoutStoreQuery *query,
    UmiWorkbenchLayoutStoreList *out_list)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL || query == NULL || out_list == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_store_list(&bridge->adapter, query, out_list);
    bridge->last_status = status;
    bridge->revision += 1U;
    return status;
}
