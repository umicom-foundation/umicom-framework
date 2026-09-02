/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_view_presentation.c
 *
 * PURPOSE:
 *   Verify that a registered view factory can be materialised into an immutable
 *   presentation snapshot and that typed properties can be formatted safely.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/ui/view_presentation.h"

/*
 * Exercise create demo view and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiStatus create_demo_view(const char *view_id,
                                  void *user_data,
                                  UmiUiViewModel **out_view)
{
    UmiUiValue value;
    UmiStatus status;
    (void)user_data;

    status = umi_ui_view_model_create(
        view_id, "test.presentation", UMI_UI_ROLE_PANE, out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ui_value_set_string(&value, "Presentation Demo");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_view_model_set_property(*out_view, "title", &value);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_value_set_integer(&value, 42);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_view_model_set_property(*out_view, "items", &value);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

/*
 * Exercise create full property view and return a clear result when the behaviour no
 * longer matches its contract.
 */
static UmiStatus create_full_property_view(const char *view_id,
                                           void *user_data,
                                           UmiUiViewModel **out_view)
{
    UmiUiValue value;
    UmiStatus status;
    size_t index;
    (void)user_data;

    status = umi_ui_view_model_create(
        view_id, "test.presentation.full", UMI_UI_ROLE_PANE, out_view);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK &&
         index < UMI_UI_PROPERTY_MAX; ++index) {
        char key[UMI_UI_PROPERTY_KEY_CAPACITY];
        const int written = snprintf(key, sizeof(key), "property.%03zu", index);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(key)) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        status = umi_ui_value_set_integer(&value, (int64_t)index);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK)
            status = umi_ui_view_model_set_property(*out_view, key, &value);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK && out_view != NULL && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiViewFactoryRegistry *registry = NULL;
    UmiUiViewFactoryDescriptor descriptor = {0};
    UmiUiViewPresentation *presentation =
        (UmiUiViewPresentation *)calloc(1U, sizeof(*presentation));
    UmiUiPropertySnapshot property;
    char text[64];

    assert(presentation != NULL);
    descriptor.create = create_demo_view;
    (void)strcpy(descriptor.view_type, "test.presentation");
    (void)strcpy(descriptor.provider_id, "org.umicom.framework.tests");

    assert(umi_ui_view_factory_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_ui_view_factory_register(registry, &descriptor) == UMI_STATUS_OK);

    assert(umi_ui_view_presentation_build(
               registry,
               "test.presentation",
               "test.presentation.instance",
               presentation) == UMI_STATUS_OK);

    assert(strcmp(presentation->view.view_type, "test.presentation") == 0);
    assert(presentation->property_count == 2U);

    assert(umi_ui_view_presentation_find_property(
               presentation, "items", &property) == UMI_STATUS_OK);
    assert(property.value.kind == UMI_UI_VALUE_INTEGER);
    assert(property.value.integer_value == 42);

    assert(umi_ui_view_presentation_value_text(
               &property.value, text, sizeof(text)) == UMI_STATUS_OK);
    assert(strcmp(text, "42") == 0);

    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.create = create_full_property_view;
    (void)strcpy(descriptor.view_type, "test.presentation.full");
    (void)strcpy(descriptor.provider_id, "org.umicom.framework.tests");
    assert(umi_ui_view_factory_register(registry, &descriptor) == UMI_STATUS_OK);
    assert(umi_ui_view_presentation_build(
               registry,
               "test.presentation.full",
               "test.presentation.full.instance",
               presentation) == UMI_STATUS_OK);
    assert(presentation->property_count == UMI_UI_PROPERTY_MAX);
    assert(umi_ui_view_presentation_find_property(
               presentation, "property.127", &property) == UMI_STATUS_OK);
    assert(property.value.integer_value == 127);

    umi_ui_view_factory_registry_destroy(registry);
    free(presentation);
    return 0;
}
