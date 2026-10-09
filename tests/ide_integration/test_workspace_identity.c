/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_workspace_identity.c
 * PURPOSE: Exercise closed, opened and rejected workspace identities without filesystem side
 * effects. AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ide_integration/platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            failed = 1;                                                                            \
            goto cleanup;                                                                          \
        }                                                                                          \
    } while (0)

/* Read the named gate rather than relying on its position in a growing report. */
static UmiIdeWorkflowGateState WorkspaceGate(const UmiIdeWorkflowReport *report)
{
    for (size_t index = 0U; index < report->gate_count; ++index)
        if (strcmp(report->gates[index].gate_id, "workspace") == 0)
            return report->gates[index].state;
    return UMI_IDE_GATE_UNKNOWN;
}
int main(void)
{
    int failed = 0;
    UmiIdeIntegrationBindings bindings;
    UmiIdeIntegrationPlatform *platform = NULL;
    /* Snapshots grow as services are integrated; keep large fixtures off the
     * small native Windows stack used by ordinary application executables. */
    UmiIdeIntegrationPlatformSnapshot *snapshot = calloc(1U, sizeof(*snapshot));
    umi_ide_integration_bindings_init(&bindings);
    CHECK(snapshot != NULL);
    CHECK(umi_ide_integration_platform_create("", &bindings, &platform) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ide_integration_platform_create_closed(&bindings, &platform) == UMI_STATUS_OK);
    CHECK(umi_ide_integration_platform_snapshot(platform, snapshot) == UMI_STATUS_OK);
    CHECK(snapshot->context.workspace_root[0] == '\0');
    CHECK(WorkspaceGate(&snapshot->workflow) == UMI_IDE_GATE_BLOCK);
    CHECK(umi_ide_integration_platform_set_workspace(platform, "/chosen/project") == UMI_STATUS_OK);
    CHECK(umi_ide_integration_platform_snapshot(platform, snapshot) == UMI_STATUS_OK);
    CHECK(strcmp(snapshot->context.workspace_root, "/chosen/project") == 0);
    CHECK(WorkspaceGate(&snapshot->workflow) == UMI_IDE_GATE_PASS);
    uint64_t revision = snapshot->revision;
    char oversized[UMI_IDE_INTEGRATION_PATH_CAPACITY + 1U];
    memset(oversized, 'x', sizeof(oversized) - 1U);
    oversized[sizeof(oversized) - 1U] = '\0';
    CHECK(umi_ide_integration_platform_set_workspace(platform, oversized) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_ide_integration_platform_set_workspace(platform, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(strcmp(umi_ide_integration_platform_workspace_root(platform), "/chosen/project") == 0);
    CHECK(umi_ide_integration_platform_snapshot(platform, snapshot) == UMI_STATUS_OK);
    CHECK(snapshot->revision == revision);
    CHECK(WorkspaceGate(&snapshot->workflow) == UMI_IDE_GATE_PASS);
    CHECK(umi_ide_integration_platform_set_workspace(platform, "") == UMI_STATUS_OK);
    CHECK(umi_ide_integration_platform_snapshot(platform, snapshot) == UMI_STATUS_OK);
    CHECK(snapshot->context.workspace_root[0] == '\0');
    CHECK(WorkspaceGate(&snapshot->workflow) == UMI_IDE_GATE_BLOCK);
cleanup:
    umi_ide_integration_platform_destroy(platform);
    free(snapshot);
    return failed;
}
