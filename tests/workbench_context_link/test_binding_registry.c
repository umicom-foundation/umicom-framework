/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_binding_registry.c
 *
 * PURPOSE:
 *   Verify deterministic panel binding registration and panel detachment.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/workbench_context_link/binding_registry.h"

int main(void)
{
    UmiWorkbenchContextLinkBindingRegistry registry;
    UmiWorkbenchContextLinkPanelBinding binding;
    umi_workbench_context_link_binding_registry_init(&registry);
    umi_workbench_context_link_panel_binding_init(&binding, "binding");
    assert(umi_workbench_context_link_panel_binding_set_primary(&binding, "editor") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_panel_binding_set_secondary(&binding, "studio") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_copy_text(binding.group_id, sizeof(binding.group_id), "blue") == UMI_STATUS_OK);
    binding.context_kind = UMI_CONTEXT_KIND_SOURCE_LOCATION;
    binding.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    assert(umi_workbench_context_link_binding_registry_upsert(&registry, &binding) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_binding_registry_count(&registry) == 1U);
    assert(umi_workbench_context_link_binding_registry_remove_panel(&registry, "editor") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_binding_registry_count(&registry) == 0U);
    umi_workbench_context_link_binding_registry_destroy(&registry);
    return 0;
}
