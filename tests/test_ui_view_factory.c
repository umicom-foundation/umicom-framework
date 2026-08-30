/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_view_factory.c
 *
 * PURPOSE:
 *   Verify view factories create Framework-owned view models by stable type.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static UmiStatus create_view(const char *view_id, void *user_data, UmiUiViewModel **out_view)
{
    (void)user_data;
    return umi_ui_view_model_create(view_id, "test.view", UMI_UI_ROLE_GENERIC, out_view);
}
int main(void)
{
    UmiUiViewFactoryRegistry *registry = NULL;
    UmiUiViewFactoryDescriptor descriptor = {0};
    UmiUiViewModel *view = NULL;
    (void)snprintf(descriptor.view_type, sizeof(descriptor.view_type), "%s", "test.view");
    (void)snprintf(descriptor.provider_id, sizeof(descriptor.provider_id), "%s", "test.provider");
    descriptor.create = create_view;
    assert(umi_ui_view_factory_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_ui_view_factory_register(registry, &descriptor) == UMI_STATUS_OK);
    assert(umi_ui_view_factory_create_view(registry, "test.view", "test.instance", &view) == UMI_STATUS_OK);
    assert(view != NULL);
    umi_ui_view_model_destroy(view);
    umi_ui_view_factory_registry_destroy(registry);
    return EXIT_SUCCESS;
}
