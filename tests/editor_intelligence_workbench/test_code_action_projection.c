/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_code_action_projection.c
 *
 * PURPOSE:
 *   Verify provider-ranked actions, applicability and selection projection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/intelligence_workbench/code_action_projection.h"

static UmiEditorRankedCodeAction action(
    const char *id, const char *title, int enabled, int preferred, int score)
{
    UmiEditorRankedCodeAction item;

    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_EDITOR_CODE_ACTION_QUERY_API_VERSION;
    item.action.struct_size = (uint32_t)sizeof(item.action);
    item.action.api_version = 1U;
    (void)strcpy(item.action.id, id);
    (void)strcpy(item.action.document_id, "main");
    (void)strcpy(item.action.title, title);
    (void)strcpy(item.action.kind, "quickfix");
    (void)strcpy(item.action.command_id, "editor.refactor.apply");
    item.action.enabled = enabled;
    item.action.preferred = preferred;
    (void)strcpy(item.provider_id, "clangd");
    item.kind_mask = UMI_EDITOR_CODE_ACTION_KIND_QUICK_FIX;
    item.relevance_score = score;
    item.safe = 1;
    item.supports_preview = 1;
    return item;
}

int main(void)
{
    UmiEditorCodeActionOrchestration *orchestration = NULL;
    UmiEditorCodeActionProviderItem provider;
    UmiEditorCodeActionQueryRequest request;
    UmiEditorCodeActionQuery *query;
    UmiEditorRankedCodeAction preferred =
        action("remove-unused", "Remove unused", 1, 1, 100);
    UmiEditorRankedCodeAction disabled =
        action("disabled-action", "Disabled action", 0, 0, 10);
    UmiEditorIntelCodeActionProjection projection;

    (void)memset(&provider, 0, sizeof(provider));
    provider.struct_size = (uint32_t)sizeof(provider);
    provider.api_version = UMI_EDITOR_CODE_ACTION_PROVIDER_API_VERSION;
    (void)strcpy(provider.id, "clangd");
    (void)strcpy(provider.label, "Clangd");
    (void)strcpy(provider.language_id, "c");
    provider.kind_mask = UMI_EDITOR_CODE_ACTION_KIND_QUICK_FIX;
    provider.enabled = 1;
    (void)memset(&request, 0, sizeof(request));
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_EDITOR_CODE_ACTION_QUERY_API_VERSION;
    (void)strcpy(request.id, "actions-main");
    (void)strcpy(request.document_id, "main");
    (void)strcpy(request.document_uri, "file:///src/main.c");
    (void)strcpy(request.language_id, "c");
    request.requested_kind_mask = UMI_EDITOR_CODE_ACTION_KIND_QUICK_FIX;
    request.trigger = UMI_EDITOR_CODE_ACTION_TRIGGER_INVOKED;
    request.include_disabled = 1;

    assert(umi_editor_code_action_orchestration_create(&orchestration) ==
           UMI_STATUS_OK);
    assert(umi_editor_code_action_provider_registry_upsert(
               umi_editor_code_action_orchestration_providers(orchestration),
               &provider) == UMI_STATUS_OK);
    query = umi_editor_code_action_orchestration_query(orchestration);
    assert(umi_editor_code_action_query_begin(query, &request) == UMI_STATUS_OK);
    assert(umi_editor_code_action_query_add(
               query,
               umi_editor_code_action_orchestration_providers(orchestration),
               &disabled) == UMI_STATUS_OK);
    assert(umi_editor_code_action_query_add(
               query,
               umi_editor_code_action_orchestration_providers(orchestration),
               &preferred) == UMI_STATUS_OK);
    assert(umi_editor_code_action_query_finalize(query) == UMI_STATUS_OK);
    assert(umi_editor_intel_code_action_projection_init(&projection) ==
           UMI_STATUS_OK);
    assert(umi_editor_intel_code_action_projection_refresh(
               &projection, orchestration) == UMI_STATUS_OK);
    assert(projection.menu.count == 2U);
    assert(projection.applicability[0] ==
           UMI_EDITOR_INTEL_APPLICABILITY_PREFERRED);
    assert(umi_editor_intel_code_action_projection_select(&projection, 1U) ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(umi_editor_intel_code_action_projection_selected(&projection) ==
           &projection.menu.items[0]);
    assert(umi_editor_intel_code_action_projection_valid(&projection));
    umi_editor_code_action_orchestration_destroy(orchestration);
    return 0;
}
