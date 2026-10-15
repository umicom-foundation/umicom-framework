/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_binding_registry.c
 *
 * PURPOSE:
 *   Verify deterministic panel binding registration and panel detachment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../value_archive/revision_cases.h"
#include <assert.h>
#include "umicom/workbench_context_link/binding_registry.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
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
    UMI_TEST_REVISION_EXHAUSTION(UmiWorkbenchContextLinkBindingRegistry, &registry,
        (umi_workbench_context_link_binding_registry_upsert(&registry, &binding)));
    /* Replacing an existing child must not wrap its independent token. */
    {
        uint64_t input_revision = binding.revision;
        uint64_t owner_revision = registry.revision;
        unsigned char child_before[sizeof(registry.items[0])];
        memcpy(child_before, &registry.items[0], sizeof(child_before));
        binding.revision = UINT64_MAX;
        UmiStatus replace_status = umi_workbench_context_link_binding_registry_upsert(&registry, &binding);
        binding.revision = input_revision;
        if (replace_status != UMI_STATUS_CAPACITY_EXCEEDED ||
            registry.revision != owner_revision ||
            memcmp(child_before, &registry.items[0], sizeof(child_before)) != 0)
            return 1;
    }

    assert(umi_workbench_context_link_binding_registry_count(&registry) == 1U);
    assert(umi_workbench_context_link_binding_registry_remove_panel(&registry, "editor") == UMI_STATUS_OK);
    UMI_TEST_REVISION_EXHAUSTION(UmiWorkbenchContextLinkBindingRegistry, &registry,
        (umi_workbench_context_link_binding_registry_remove_panel(&registry, "editor")));

    assert(umi_workbench_context_link_binding_registry_count(&registry) == 0U);
    umi_workbench_context_link_binding_registry_destroy(&registry);
    return 0;
}
