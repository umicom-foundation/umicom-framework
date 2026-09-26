/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/ai_workspace_main.c
 *
 * PURPOSE:
 *   Follow a source-backed drafting and reviewed tool workflow without a network, model download or persistent file.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/providers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiAiRuntime runtime; UmiDataServer *data = NULL; UmiAiWorkspace *workspace = NULL;
    UmiAiProvider provider; UmiAiWorkspaceJob *job = calloc(1U, sizeof(*job)); UmiStatus status;
    umi_ai_runtime_init(&runtime);
    if (job == NULL) return 1;
    status = umi_data_server_create_memory(&data);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceExtractiveProviderCreate(&provider);
    if (status == UMI_STATUS_OK) status = umi_ai_provider_registry_add(&runtime.providers, &provider);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceCreate(data, &runtime, "lesson", &workspace);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspacePutCollection(workspace, "workshop", "Community workshop");
    if (status == UMI_STATUS_OK) status = UmiAiWorkspacePutSource(workspace, "opening", "workshop", "Opening notice",
        "The community workshop opens at 10:00 on Saturday.\nBring a notebook.", 7U);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspacePrepare(workspace, "notice.draft", UMI_AI_WORKSPACE_GROUNDED_DRAFT,
        "umicom.extractive-preview", "extractive-preview", "workshop", "When does the workshop open?", "writer", 256U);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceReview(workspace, "notice.draft", "reviewer", true);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceRun(workspace, "notice.draft", NULL);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceJobFind(workspace, "notice.draft", job);
    if (status == UMI_STATUS_OK) {
        printf("%s\nSource: %s, lines %u-%u\n", job->response.text, job->evidence[0].source.title,
            job->evidence[0].source.firstLine, job->evidence[0].source.lastLine);
        if (strstr(job->response.text, "10:00") == NULL || job->state != UMI_AI_WORKSPACE_SUCCEEDED) status = UMI_STATUS_INTERNAL_ERROR;
    }
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceRegisterCapacityTool(&runtime);
    runtime.policy.allow_tools = 1;
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceAllowTool(workspace, "umicom.workspace.capacities");
    if (status == UMI_STATUS_OK) status = UmiAiWorkspacePrepare(workspace, "limits.query", UMI_AI_WORKSPACE_TOOL,
        "umicom.workspace.capacities", NULL, NULL, "{}", "writer", 0U);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceReview(workspace, "limits.query", "reviewer", true);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceRun(workspace, "limits.query", NULL);
    if (status == UMI_STATUS_OK) status = UmiAiWorkspaceJobFind(workspace, "limits.query", job);
    if (status == UMI_STATUS_OK) printf("Reviewed read-only tool: %s\nPractice complete. No network request or file write was made.\n", job->response.text);
    else fprintf(stderr, "AI workspace example failed with status %d.\n", (int)status);
    UmiAiWorkspaceDestroy(workspace); umi_data_server_destroy(data); umi_ai_runtime_destroy(&runtime); free(job);
    return status == UMI_STATUS_OK ? 0 : 1;
}
