/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_view_presentation.c
 *
 * PURPOSE:
 *   Verify that a registered view factory can be materialised into an immutable
 *   presentation snapshot and that typed properties can be formatted safely.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/ui/view_presentation.h"

static UmiStatus create_demo_view(const char *view_id,
                                  void *user_data,
                                  UmiUiViewModel **out_view)
{
    UmiUiValue value;
    UmiStatus status;
    (void)user_data;

    status = umi_ui_view_model_create(
        view_id, "test.presentation", UMI_UI_ROLE_PANE, out_view);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ui_value_set_string(&value, "Presentation Demo");
    if (status == UMI_STATUS_OK) {
        status = umi_ui_view_model_set_property(*out_view, "title", &value);
    }

    if (status == UMI_STATUS_OK) {
        status = umi_ui_value_set_integer(&value, 42);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_view_model_set_property(*out_view, "items", &value);
    }

    if (status != UMI_STATUS_OK) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

int main(void)
{
    UmiUiViewFactoryRegistry *registry = NULL;
    UmiUiViewFactoryDescriptor descriptor = {0};
    UmiUiViewPresentation presentation;
    UmiUiPropertySnapshot property;
    char text[64];

    descriptor.create = create_demo_view;
    (void)strcpy(descriptor.view_type, "test.presentation");
    (void)strcpy(descriptor.provider_id, "org.umicom.framework.tests");

    assert(umi_ui_view_factory_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_ui_view_factory_register(registry, &descriptor) == UMI_STATUS_OK);

    assert(umi_ui_view_presentation_build(
               registry,
               "test.presentation",
               "test.presentation.instance",
               &presentation) == UMI_STATUS_OK);

    assert(strcmp(presentation.view.view_type, "test.presentation") == 0);
    assert(presentation.property_count == 2U);

    assert(umi_ui_view_presentation_find_property(
               &presentation, "items", &property) == UMI_STATUS_OK);
    assert(property.value.kind == UMI_UI_VALUE_INTEGER);
    assert(property.value.integer_value == 42);

    assert(umi_ui_view_presentation_value_text(
               &property.value, text, sizeof(text)) == UMI_STATUS_OK);
    assert(strcmp(text, "42") == 0);

    umi_ui_view_factory_registry_destroy(registry);
    return 0;
}
