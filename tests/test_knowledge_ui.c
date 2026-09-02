/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_knowledge_ui.c
 * PURPOSE: Verify toolkit-neutral Knowledge Centre view projections.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/knowledge_ui/views.h"
#include "umicom/ui/command_view.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiKnowledgeServiceConfig config = umi_knowledge_service_config_default();
    UmiKnowledgeService *service = NULL;
    UmiKnowledgeCollection collection;
    UmiUiViewModel *view = NULL;
    UmiUiValue value;
    UmiUiCommandViewAction action;
    config.source_capacity = 4U;
    config.vector_capacity = 8U;
    assert(umi_knowledge_service_create(&config, &service) == UMI_STATUS_OK);
    assert(umi_knowledge_collection_init(
        &collection, "project", "Project", "Active project")
        == UMI_STATUS_OK);
    assert(umi_knowledge_service_add_collection(service, &collection)
           == UMI_STATUS_OK);
    assert(umi_knowledge_ui_overview_view_create(
        "test.knowledge", service, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
        view, "knowledge.collections", &value) == UMI_STATUS_OK);
    assert(value.integer_value == 1);
    assert(umi_ui_command_view_action_at(view, 0U, &action) == UMI_STATUS_OK);
    assert(strcmp(action.action_id, "studio.action.knowledge.search") == 0);
    umi_ui_view_model_destroy(view);
    assert(umi_knowledge_ui_collections_view_create(
        "test.collections", service, &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    umi_knowledge_service_destroy(service);
    return 0;
}
