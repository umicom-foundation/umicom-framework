/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection_provider/test_registry.c
 *
 * PURPOSE:
 *   Verify provider catalogue replacement, panel lookup and removal.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/workbench_selection_provider/registry.h"

int main(void)
{
    UmiWorkbenchSelectionProviderRegistry registry;
    UmiWorkbenchSelectionProviderDescriptor descriptor;
    umi_workbench_selection_provider_registry_init(&registry);
    umi_workbench_selection_provider_descriptor_init(
        &descriptor, "provider");
    assert(umi_workbench_selection_provider_descriptor_set_identity(
        &descriptor, "application", "panel", "Provider") == UMI_STATUS_OK);
    assert(umi_workbench_selection_provider_registry_upsert(
        &registry, &descriptor) == UMI_STATUS_OK);
    assert(registry.count == 1U);
    assert(umi_workbench_selection_provider_registry_find_panel(
        &registry, "panel") != NULL);
    descriptor.state = UMI_WORKBENCH_SELECTION_PROVIDER_ACTIVE;
    assert(umi_workbench_selection_provider_registry_upsert(
        &registry, &descriptor) == UMI_STATUS_OK);
    assert(registry.count == 1U);
    assert(umi_workbench_selection_provider_registry_remove(
        &registry, "provider") == UMI_STATUS_OK);
    umi_workbench_selection_provider_registry_destroy(&registry);

    return 0;
}
