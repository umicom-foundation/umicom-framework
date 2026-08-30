/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_code_action_provider.c
 *
 * PURPOSE:
 *   Implement the test editor code action provider behavior for
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
 * File: tests/editor/test_editor_code_action_provider.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/code_action_provider.h"

int main(void)
{
    UmiEditorCodeActionProviderRegistry *registry = NULL;
    UmiEditorCodeActionProviderItem provider = {0};
    UmiEditorCodeActionProviderSnapshot snapshot;

    provider.struct_size = (uint32_t)sizeof(provider);
    provider.api_version = UMI_EDITOR_CODE_ACTION_PROVIDER_API_VERSION;
    (void)strcpy(provider.id, "clangd");
    (void)strcpy(provider.label, "Clangd Code Actions");
    (void)strcpy(provider.language_id, "c");
    provider.kind_mask = UMI_EDITOR_CODE_ACTION_KIND_QUICK_FIX |
                         UMI_EDITOR_CODE_ACTION_KIND_REFACTOR;
    provider.priority = 200;
    provider.supports_diagnostics = 1;
    provider.supports_selection = 1;
    provider.supports_resolution = 1;
    provider.supports_workspace_edits = 1;
    provider.supports_preview = 1;
    provider.enabled = 1;
    assert(umi_editor_code_action_provider_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_editor_code_action_provider_registry_upsert(registry,
                                                           &provider) ==
           UMI_STATUS_OK);
    assert(umi_editor_code_action_provider_supports(
        &provider, "c", UMI_EDITOR_CODE_ACTION_KIND_QUICK_FIX));
    assert(!umi_editor_code_action_provider_supports(
        &provider, "java", UMI_EDITOR_CODE_ACTION_KIND_QUICK_FIX));
    assert(umi_editor_code_action_provider_registry_snapshot(registry,
                                                             &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.provider_count == 1U);
    assert(snapshot.enabled_provider_count == 1U);
    assert(snapshot.language_count == 1U);
    assert(umi_editor_code_action_provider_registry_remove(registry,
                                                           "clangd") ==
           UMI_STATUS_OK);
    assert(umi_editor_code_action_provider_registry_count(registry) == 0U);
    umi_editor_code_action_provider_registry_destroy(registry);
    return 0;
}
