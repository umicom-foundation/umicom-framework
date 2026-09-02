/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_authoring_tools.c
 *
 * PURPOSE:
 *   Exercise Batch 25 reusable platform contracts with deterministic smoke coverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <string.h>

#include "umicom/designer/designer.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesignerPropertySchemaRegistry *properties = NULL;
    UmiDesignerPropertySchemaSnapshot property = {0};
    UmiDesignerSignalBindingRegistry *signals = NULL;
    UmiDesignerSignalBindingSnapshot signal = {0};
    UmiDesignerAuthoringSession *session = NULL;
    UmiDesignerAuthoringSessionSnapshot session_snapshot;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_designer_property_schema_registry_create(&properties) != UMI_STATUS_OK) return 1;
    strcpy(property.id, "button.text");
    strcpy(property.component_type, "button");
    strcpy(property.property_name, "text");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_designer_property_schema_registry_upsert(properties, &property) != UMI_STATUS_OK) return 2;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_designer_signal_binding_registry_create(&signals) != UMI_STATUS_OK) return 3;
    strcpy(signal.id, "button.clicked");
    strcpy(signal.node_id, "button");
    strcpy(signal.signal_name, "clicked");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_designer_signal_binding_registry_upsert(signals, &signal) != UMI_STATUS_OK) return 4;

    /*
     * The aggregate designer header must expose the complete authoring session.
     * Studio Platform Shell consumes this type directly.
     */
    if (umi_designer_authoring_session_create(&session) != UMI_STATUS_OK) return 5;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_designer_authoring_session_snapshot(
            session, &session_snapshot) != UMI_STATUS_OK) return 6;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session_snapshot.api_version == 0U) return 7;

    umi_designer_authoring_session_destroy(session);
    umi_designer_signal_binding_registry_destroy(signals);
    umi_designer_property_schema_registry_destroy(properties);
    return 0;
}
