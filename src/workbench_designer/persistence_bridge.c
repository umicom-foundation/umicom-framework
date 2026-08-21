/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/persistence_bridge.c
 *
 * PURPOSE:
 *   Apply optimistic revisions and explicit save-state transitions around the
 *   existing layout store adapter.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/persistence_bridge.h"
#include "internal.h"


UmiStatus umi_workbench_designer_persistence_bridge_init(
    UmiWorkbenchDesignerPersistenceBridge *bridge,
    const UmiWorkbenchLayoutStoreAdapter *adapter)
{
    UmiStatus status;
    if (bridge == NULL || adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_layout_store_adapter_validate(adapter);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(bridge, 0, sizeof(*bridge));
    bridge->adapter = *adapter;
    bridge->save_state = UMI_WORKBENCH_DESIGNER_SAVE_CLEAN;
    bridge->last_status = UMI_STATUS_OK;
    return UMI_STATUS_OK;
}

void umi_workbench_designer_persistence_bridge_mark_dirty(
    UmiWorkbenchDesignerPersistenceBridge *bridge)
{
    if (bridge == NULL) return;
    if (bridge->save_state != UMI_WORKBENCH_DESIGNER_SAVE_SAVING) {
        bridge->save_state = UMI_WORKBENCH_DESIGNER_SAVE_DIRTY;
    }
    bridge->revision += 1U;
}

UmiStatus umi_workbench_designer_persistence_bridge_save(
    UmiWorkbenchDesignerPersistenceBridge *bridge,
    UmiWorkbenchLayoutDocument *document,
    uint64_t expected_revision,
    uint64_t timestamp_ms)
{
    uint64_t resulting_revision = 0U;
    UmiStatus status;
    if (bridge == NULL || document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    bridge->save_state = UMI_WORKBENCH_DESIGNER_SAVE_SAVING;
    bridge->last_save_started_ms = timestamp_ms;
    bridge->last_error[0] = '\0';
    bridge->revision += 1U;
    status = umi_workbench_layout_store_save(
        &bridge->adapter, document, expected_revision, &resulting_revision);
    bridge->last_status = status;
    if (status == UMI_STATUS_OK) {
        bridge->persisted_revision = resulting_revision;
        bridge->last_save_completed_ms = timestamp_ms;
        bridge->save_state = UMI_WORKBENCH_DESIGNER_SAVE_CLEAN;
        document->version.revision = resulting_revision;
        document->version.base_revision = resulting_revision;
        (void)umi_workbench_layout_document_set_flag(
            document, UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY, false);
    } else {
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

UmiStatus umi_workbench_designer_persistence_bridge_load(
    UmiWorkbenchDesignerPersistenceBridge *bridge,
    const char *layout_id,
    UmiWorkbenchLayoutDocument *out_document)
{
    UmiStatus status;
    if (bridge == NULL || layout_id == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_store_load(
        &bridge->adapter, layout_id, out_document);
    bridge->last_status = status;
    if (status == UMI_STATUS_OK) {
        bridge->persisted_revision = out_document->version.revision;
        bridge->save_state = UMI_WORKBENCH_DESIGNER_SAVE_CLEAN;
        bridge->last_error[0] = '\0';
    }
    bridge->revision += 1U;
    return status;
}

UmiStatus umi_workbench_designer_persistence_bridge_delete(
    UmiWorkbenchDesignerPersistenceBridge *bridge,
    const char *layout_id,
    uint64_t expected_revision)
{
    UmiStatus status;
    if (bridge == NULL || layout_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_layout_store_delete(
        &bridge->adapter, layout_id, expected_revision);
    bridge->last_status = status;
    bridge->revision += 1U;
    return status;
}

UmiStatus umi_workbench_designer_persistence_bridge_list(
    UmiWorkbenchDesignerPersistenceBridge *bridge,
    const UmiWorkbenchLayoutStoreQuery *query,
    UmiWorkbenchLayoutStoreList *out_list)
{
    UmiStatus status;
    if (bridge == NULL || query == NULL || out_list == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_store_list(&bridge->adapter, query, out_list);
    bridge->last_status = status;
    bridge->revision += 1U;
    return status;
}
