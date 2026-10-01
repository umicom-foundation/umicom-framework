/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_editor_publication.c
 * PURPOSE: Keep compact editor mutations from wrapping their observation token.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
/* Only this fixture needs direct counter access. Including the real owner
 * keeps production interfaces free of a test-only counter setter. */
#include "../../src/editor/configuration.c"

#define REQUIRE(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); return 1; \
} } while (0)

int main(void)
{
    UmiEditorConfigurationRegistry *registry = NULL;
    REQUIRE(umi_editor_configuration_registry_create(&registry) == UMI_STATUS_OK);
    UmiEditorConfigurationSnapshot input = {0}, output;
    (void)snprintf(input.id, sizeof(input.id), "%s", "editor.settings");
    REQUIRE(umi_editor_configuration_registry_upsert(registry, &input) == UMI_STATUS_OK);
    registry->revision = UINT64_MAX;
    UmiSnapshotCapture capture;
    REQUIRE(umi_editor_configuration_registry_capture(registry, &output, 1U, &capture) == UMI_STATUS_OK);
    REQUIRE(capture.revision == UINT64_MAX && capture.count == 1U);

    /* Removal is a mutation too. It must preserve the last readable state at
     * exhaustion, even though the requested record exists and could be removed. */
    REQUIRE(umi_editor_configuration_registry_remove(registry, input.id) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(umi_editor_configuration_registry_upsert(registry, &input) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(umi_editor_configuration_registry_replace_if_current(
        registry, capture.revision, NULL, 0U, NULL) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(umi_editor_configuration_registry_count(registry) == 1U);
    REQUIRE(umi_editor_configuration_registry_revision(registry) == UINT64_MAX);
    REQUIRE(umi_editor_configuration_registry_find(registry, input.id, &output) == UMI_STATUS_OK);
    REQUIRE(strcmp(output.id, input.id) == 0);
    umi_editor_configuration_registry_destroy(registry);
    return 0;
}
