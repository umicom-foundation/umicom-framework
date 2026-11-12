/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_consumers/ui_components.cpp
 *
 * PURPOSE:
 *   Exercise C component values and registry ownership from a C++ application without a GUI toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/



#include "umicom/ui/components/accessibility.h"
#include "umicom/ui/components/registry.h"
#include "umicom/ui/components/selection.h"
#include "umicom/ui/components/state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #expression); return 1; \
} } while (0)

/* The large registry has one explicit owner. Framework receives borrowed values
 * and copies specifications; no C++ object or exception crosses its C API. */
static int exercise(UmiUiComponentRegistry *registry)
{
    UmiUiComponentSpec button = umi_ui_component_spec_default(UMI_UI_COMPONENT_BUTTON);
    UmiUiComponentSpec selected = {};
    UmiUiComponentAccessibility accessibility = {};
    UmiUiComponentState state = umi_ui_component_state_default();
    UmiUiComponentSelection selection = umi_ui_component_selection_none();

    CHECK(state.visible && state.enabled && !state.busy && !state.has_error);
    CHECK(!selection.has_selection);
    CHECK(umi_ui_component_spec_set_id(&button, "save-record") == UMI_STATUS_OK);
    CHECK(umi_ui_component_spec_set_text(&button, "Save record") == UMI_STATUS_OK);
    CHECK(umi_ui_component_spec_validate(&button) == UMI_STATUS_OK);
    CHECK(umi_ui_component_accessibility_set(&accessibility,
        "Save record", "Save the current local record") == UMI_STATUS_OK);
    CHECK(strcmp(accessibility.name, "Save record") == 0);
    CHECK(strcmp(accessibility.description, "Save the current local record") == 0);

    umi_ui_component_registry_init(registry);
    CHECK(umi_ui_component_registry_add(registry, &button) == UMI_STATUS_OK);
    CHECK(registry->count == 1U);
    CHECK(umi_ui_component_spec_set_text(&button, "Changed input") == UMI_STATUS_OK);
    CHECK(umi_ui_component_registry_find(registry, "save-record", &selected) == UMI_STATUS_OK);
    CHECK(strcmp(selected.text, "Save record") == 0);
    CHECK(selected.kind == UMI_UI_COMPONENT_BUTTON);
    CHECK(umi_ui_component_registry_add(registry, &button) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(registry->count == 1U);
    CHECK(umi_ui_component_registry_find(registry, "missing", &selected) == UMI_STATUS_NOT_FOUND);
    return 0;
}

int main()
{
    UmiUiComponentRegistry *registry =
        static_cast<UmiUiComponentRegistry *>(malloc(sizeof(UmiUiComponentRegistry)));
    if (registry == nullptr) return 2;
    const int result = exercise(registry);
    free(registry);
    if (result == 0) (void)puts("Umicom C UI component ownership checked from C++.");
    return result;
}
