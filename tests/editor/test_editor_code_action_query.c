/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_code_action_query.c
 *
 * PURPOSE:
 *   Implement the test editor code action query behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_code_action_query.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/code_action_query.h"

static UmiEditorRankedCodeAction action(const char *id, const char *title,
                                        int preferred, int score)
{
    UmiEditorRankedCodeAction item = {0};
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_EDITOR_CODE_ACTION_QUERY_API_VERSION;
    item.action.struct_size = (uint32_t)sizeof(item.action);
    item.action.api_version = 1U;
    (void)strcpy(item.action.id, id);
    (void)strcpy(item.action.document_id, "main");
    (void)strcpy(item.action.title, title);
    (void)strcpy(item.action.kind, "quickfix");
    (void)strcpy(item.action.command_id, "editor.refactor.apply");
    item.action.enabled = 1;
    item.action.preferred = preferred;
    (void)strcpy(item.provider_id, "clangd");
    (void)strcpy(item.diagnostic_id, "unused-variable");
    item.kind_mask = UMI_EDITOR_CODE_ACTION_KIND_QUICK_FIX;
    item.relevance_score = score;
    item.safe = 1;
    item.supports_preview = 1;
    return item;
}

int main(void)
{
    UmiEditorCodeActionProviderRegistry *providers = NULL;
    UmiEditorCodeActionQuery *query = NULL;
    UmiEditorCodeActionProviderItem provider = {0};
    UmiEditorCodeActionQueryRequest request = {0};
    UmiEditorRankedCodeAction first;
    UmiEditorRankedCodeAction second;
    UmiEditorRankedCodeAction result;
    UmiEditorCodeActionQuerySnapshot snapshot;

    provider.struct_size = (uint32_t)sizeof(provider);
    provider.api_version = UMI_EDITOR_CODE_ACTION_PROVIDER_API_VERSION;
    (void)strcpy(provider.id, "clangd");
    (void)strcpy(provider.label, "Clangd");
    (void)strcpy(provider.language_id, "c");
    provider.kind_mask = UMI_EDITOR_CODE_ACTION_KIND_QUICK_FIX;
    provider.priority = 50;
    provider.supports_preview = 1;
    provider.enabled = 1;
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_EDITOR_CODE_ACTION_QUERY_API_VERSION;
    (void)strcpy(request.id, "query-main");
    (void)strcpy(request.document_id, "main");
    (void)strcpy(request.document_uri, "file:///workspace/main.c");
    (void)strcpy(request.language_id, "c");
    (void)strcpy(request.diagnostic_id, "unused-variable");
    request.requested_kind_mask = UMI_EDITOR_CODE_ACTION_KIND_QUICK_FIX;
    request.trigger = UMI_EDITOR_CODE_ACTION_TRIGGER_DIAGNOSTIC;
    first = action("remove-unused", "Remove unused variable", 1, 10);
    second = action("suppress-warning", "Suppress warning", 0, 500);
    assert(umi_editor_code_action_provider_registry_create(&providers) ==
           UMI_STATUS_OK);
    assert(umi_editor_code_action_provider_registry_upsert(providers,
                                                           &provider) ==
           UMI_STATUS_OK);
    assert(umi_editor_code_action_query_create(&query) == UMI_STATUS_OK);
    assert(umi_editor_code_action_query_begin(query, &request) ==
           UMI_STATUS_OK);
    assert(umi_editor_code_action_query_add(query, providers, &second) ==
           UMI_STATUS_OK);
    assert(umi_editor_code_action_query_add(query, providers, &first) ==
           UMI_STATUS_OK);
    assert(umi_editor_code_action_query_finalize(query) == UMI_STATUS_OK);
    assert(umi_editor_code_action_query_at(query, 0U, &result) ==
           UMI_STATUS_OK);
    assert(strcmp(result.action.id, "remove-unused") == 0);
    assert(umi_editor_code_action_query_snapshot(query, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.result_count == 2U);
    assert(snapshot.preferred_count == 1U);
    assert(snapshot.safe_count == 2U);
    umi_editor_code_action_query_destroy(query);
    umi_editor_code_action_provider_registry_destroy(providers);
    return 0;
}
