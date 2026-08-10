/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_binding.c
 *
 * PURPOSE:
 *   Verify one-way property binding between toolkit-neutral view models.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void)
{
    UmiUiViewModel *source_view = NULL;
    UmiUiViewModel *target_view = NULL;
    UmiUiBindingRegistry *bindings = NULL;
    UmiUiBindingDescriptor descriptor = {0};
    UmiUiValue value;
    UmiUiValue result;
    assert(umi_ui_view_model_create("source.view", "label", UMI_UI_ROLE_GENERIC, &source_view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_create("target.view", "label", UMI_UI_ROLE_GENERIC, &target_view) == UMI_STATUS_OK);
    assert(umi_ui_value_set_string(&value, "Ready") == UMI_STATUS_OK);
    assert(umi_ui_view_model_set_property(source_view, "text", &value) == UMI_STATUS_OK);
    assert(umi_ui_binding_registry_create(&bindings) == UMI_STATUS_OK);
    (void)snprintf(descriptor.binding_id, sizeof(descriptor.binding_id), "%s", "binding.status");
    descriptor.source_view = source_view; descriptor.target_view = target_view;
    (void)snprintf(descriptor.source_property, sizeof(descriptor.source_property), "%s", "text");
    (void)snprintf(descriptor.target_property, sizeof(descriptor.target_property), "%s", "text");
    descriptor.enabled = 1;
    assert(umi_ui_binding_register(bindings, &descriptor) == UMI_STATUS_OK);
    assert(umi_ui_binding_propagate(bindings, descriptor.binding_id) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(target_view, "text", &result) == UMI_STATUS_OK);
    assert(strcmp(result.string_value, "Ready") == 0);
    umi_ui_binding_registry_destroy(bindings);
    umi_ui_view_model_destroy(target_view);
    umi_ui_view_model_destroy(source_view);
    return EXIT_SUCCESS;
}
