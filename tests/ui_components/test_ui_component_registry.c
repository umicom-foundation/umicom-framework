/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_components/test_ui_component_registry.c
 *
 * PURPOSE:
 *   Test one reusable toolkit-neutral UI component behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/registry.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #expression); return 1; \
} } while (0)


/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* The expanded heap-owned regression preserves the original add/find scenario and checks malformed storage, aliasing and refusal preservation without depending on assert evaluation. The original test remains for review. The previous implementation is retained for review. */
#if 0
int main(void)
{
    UmiUiComponentRegistry registry;
    UmiUiComponentSpec spec =
        umi_ui_component_spec_default(UMI_UI_COMPONENT_LABEL);
    UmiUiComponentSpec found;

    umi_ui_component_registry_init(&registry);
    (void)umi_ui_component_spec_set_id(&spec, "title");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ui_component_registry_add(&registry, &spec) != UMI_STATUS_OK) {
        return 1;
    }
    return umi_ui_component_registry_find(&registry, "title", &found) ==
                   UMI_STATUS_OK
               ? 0
               : 2;
}

#endif

static int exercise_registry(UmiUiComponentRegistry *registry,
                             UmiUiComponentRegistry *before)
{
    UmiUiComponentSpec spec = umi_ui_component_spec_default(UMI_UI_COMPONENT_LABEL);
    UmiUiComponentSpec output;
    UmiUiComponentSpec output_before;
    CHECK(umi_ui_component_spec_set_id(&spec, "title") == UMI_STATUS_OK);
    CHECK(umi_ui_component_spec_set_text(&spec, "Original") == UMI_STATUS_OK);
    umi_ui_component_registry_init(registry);
    CHECK(umi_ui_component_registry_validate(registry) == UMI_STATUS_OK);
    CHECK(umi_ui_component_registry_add(registry, &spec) == UMI_STATUS_OK);
    CHECK(umi_ui_component_registry_find(registry, "title", &output) == UMI_STATUS_OK);
    CHECK(strcmp(output.text, "Original") == 0);

    /* A query can borrow the output that will be replaced on success. */
    CHECK(umi_ui_component_spec_set_text(&output, "Previous output") == UMI_STATUS_OK);
    CHECK(umi_ui_component_registry_find(registry, output.id, &output) == UMI_STATUS_OK);
    CHECK(strcmp(output.text, "Original") == 0);
    (void)memcpy(&output_before, &output, sizeof(output));
    char bad_id[UMI_UI_COMPONENT_ID_CAPACITY];
    (void)memset(bad_id, 'q', sizeof(bad_id));
    CHECK(umi_ui_component_registry_find(registry, bad_id, &output) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&output, &output_before, sizeof(output)) == 0);
    CHECK(umi_ui_component_registry_find(registry, "missing", &output) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(&output, &output_before, sizeof(output)) == 0);

    (void)memcpy(before, registry, sizeof(*before));
    CHECK(umi_ui_component_registry_find(registry, "title", &registry->items[0]) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(before, registry, sizeof(*before)) == 0);

    const size_t bad_counts[] = {UMI_UI_COMPONENT_REGISTRY_CAPACITY + 1U, SIZE_MAX};
    for (size_t index = 0U; index < sizeof(bad_counts) / sizeof(bad_counts[0]); ++index) {
        registry->count = bad_counts[index];
        (void)memcpy(before, registry, sizeof(*before));
        CHECK(umi_ui_component_registry_validate(registry) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_ui_component_registry_find(registry, "title", &output) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_ui_component_registry_add(registry, &spec) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(before, registry, sizeof(*before)) == 0);
        CHECK(memcmp(&output, &output_before, sizeof(output)) == 0);
    }
    registry->count = 1U;

    /* Reject malformed later rows even when the requested first row is valid. */
    UmiUiComponentSpec second = spec;
    CHECK(umi_ui_component_spec_set_id(&second, "second") == UMI_STATUS_OK);
    registry->items[1] = second;
    registry->count = 2U;
    for (size_t field = 0U; field < 5U; ++field) {
        registry->items[1] = second;
        char *fields[] = {registry->items[1].id, registry->items[1].text,
            registry->items[1].css_class, registry->items[1].tooltip,
            registry->items[1].accessible_name};
        const size_t sizes[] = {sizeof(second.id), sizeof(second.text),
            sizeof(second.css_class), sizeof(second.tooltip), sizeof(second.accessible_name)};
        (void)memset(fields[field], 'x', sizes[field]);
        (void)memcpy(before, registry, sizeof(*before));
        CHECK(umi_ui_component_registry_find(registry, "title", &output) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_ui_component_registry_add(registry, &spec) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(before, registry, sizeof(*before)) == 0);
        CHECK(memcmp(&output, &output_before, sizeof(output)) == 0);
    }
    registry->items[1] = spec;
    CHECK(umi_ui_component_registry_validate(registry) == UMI_STATUS_INVALID_STATE);
    registry->items[1] = second;
    registry->items[1].structure_size = 0U;
    CHECK(umi_ui_component_registry_validate(registry) == UMI_STATUS_INVALID_STATE);
    registry->items[1] = second;
    registry->items[1].kind = (UmiUiComponentKind)999;
    CHECK(umi_ui_component_registry_validate(registry) == UMI_STATUS_INVALID_STATE);

    /* Inactive slots are storage, not published rows. They may supply a new
     * specification when it is captured before the owner count changes. */
    registry->count = 1U;
    (void)memset(&registry->items[1], 'x', sizeof(registry->items[1]));
    CHECK(umi_ui_component_registry_validate(registry) == UMI_STATUS_OK);
    registry->items[1] = second;
    CHECK(umi_ui_component_registry_add(registry, &registry->items[1]) == UMI_STATUS_OK);
    CHECK(registry->count == 2U);
    CHECK(strcmp(registry->items[1].id, "second") == 0);

    umi_ui_component_registry_init(registry);
    for (size_t index = 0U; index < UMI_UI_COMPONENT_REGISTRY_CAPACITY; ++index) {
        registry->items[index] = spec;
        (void)snprintf(registry->items[index].id, sizeof(spec.id), "row-%zu", index);
    }
    registry->count = UMI_UI_COMPONENT_REGISTRY_CAPACITY;
    CHECK(umi_ui_component_registry_validate(registry) == UMI_STATUS_OK);
    (void)memcpy(before, registry, sizeof(*before));
    CHECK(umi_ui_component_registry_add(registry, &registry->items[0]) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_ui_component_registry_add(registry, &spec) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(before, registry, sizeof(*before)) == 0);
    CHECK(umi_ui_component_registry_find(registry, registry->items[registry->count - 1U].id,
                                         &output) == UMI_STATUS_OK);

    CHECK(umi_ui_component_spec_set_id(&spec, "prefix") == UMI_STATUS_OK);
    CHECK(umi_ui_component_spec_set_id(&spec, spec.id + 3) == UMI_STATUS_OK);
    CHECK(strcmp(spec.id, "fix") == 0);
    (void)memcpy(&output_before, &spec, sizeof(spec));
    CHECK(umi_ui_component_spec_set_id(&spec, bad_id) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&spec, &output_before, sizeof(spec)) == 0);
    CHECK(umi_ui_component_registry_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ui_component_registry_find(NULL, "title", &output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ui_component_registry_add(registry, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}

int main(void)
{
    /* Registry-sized snapshots belong on the heap, independently of test stack limits. */
    UmiUiComponentRegistry *registry = malloc(sizeof(*registry));
    UmiUiComponentRegistry *before = malloc(sizeof(*before));
    if (registry == NULL || before == NULL) { free(before); free(registry); return 2; }
    int result = exercise_registry(registry, before);
    free(before);
    free(registry);
    return result;
}
