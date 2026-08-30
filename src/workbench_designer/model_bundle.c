/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/model_bundle.c
 *
 * PURPOSE:
 *   Capture coherent frontend state from the active service session without
 *   exposing mutable controller or document internals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/model_bundle.h"
#include "internal.h"


void umi_workbench_designer_model_bundle_init(
    UmiWorkbenchDesignerModelBundle *bundle)
{
    if (bundle == NULL) return;
    (void)memset(bundle, 0, sizeof(*bundle));
    umi_workbench_designer_breadcrumbs_init(&bundle->breadcrumbs);
    umi_workbench_designer_property_groups_init(&bundle->property_groups);
    umi_workbench_designer_validation_gate_init(&bundle->validation_gate);
    umi_workbench_designer_command_palette_init(&bundle->command_palette);
    umi_workbench_designer_browser_preview_init(&bundle->browser_preview);
}

UmiStatus umi_workbench_designer_model_bundle_capture(
    UmiWorkbenchDesignerModelBundle *bundle,
    const UmiWorkbenchDesignerController *controller,
    const UmiWorkbenchDesignerKeymap *keymap,
    uint64_t captured_at_ms)
{
    UmiWorkbenchDesignerSession *session;
    const UmiWorkbenchLayoutDocument *document;
    const UmiWorkbenchDesignerSelection *selection;
    const UmiWorkbenchDesignerPropertyModel *properties;
    const UmiWorkbenchDesignerDiagnostics *diagnostics;
    const char *active_node_id;
    UmiStatus status;
    if (bundle == NULL || controller == NULL || controller->service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_model_bundle_init(bundle);
    status = umi_workbench_designer_snapshot_capture(
        controller, &bundle->service_snapshot);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_designer_command_palette_seed(
        &bundle->command_palette, keymap);
    if (status != UMI_STATUS_OK) return status;
    session = umi_workbench_designer_service_active(controller->service);
    if (session == NULL) {
        bundle->captured_at_ms = captured_at_ms;
        bundle->revision = 1U;
        return UMI_STATUS_OK;
    }
    document = umi_workbench_designer_session_document_const(session);
    selection = umi_workbench_designer_session_selection(session);
    properties = umi_workbench_designer_session_properties(session);
    diagnostics = umi_workbench_designer_session_diagnostics(session);
    active_node_id = selection != NULL
        ? umi_workbench_designer_selection_primary(selection) : NULL;
    if (active_node_id != NULL && active_node_id[0] != '\0') {
        status = umi_workbench_designer_breadcrumbs_build(
            &bundle->breadcrumbs, document, active_node_id);
        if (status != UMI_STATUS_OK) return status;
    }
    if (properties != NULL) {
        status = umi_workbench_designer_property_groups_build(
            &bundle->property_groups, properties);
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_workbench_designer_validation_gate_evaluate(
        &bundle->validation_gate, document, diagnostics, true);
    if (status != UMI_STATUS_OK) return status;
    {
        UmiWorkbenchDesignerSize size = {260.0, 160.0};
        status = umi_workbench_designer_browser_preview_build(
            &bundle->browser_preview, document, size);
        if (status != UMI_STATUS_OK) return status;
    }
    bundle->captured_at_ms = captured_at_ms;
    bundle->revision = 1U;
    return UMI_STATUS_OK;
}
