/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/ai_source_change_main.c
 * PURPOSE: Demonstrate an explicit source replacement with a complete before-and-after review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/source_change.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
    /* Use memory for a self-contained lesson. Real applications supply their
     * existing Data Server connection and keep it alive until the workspace
     * and all work using it have finished. No model is involved in this edit. */
    UmiAiRuntime *runtime = calloc(1U, sizeof(*runtime));
    if (runtime == NULL)
        return 1;
    umi_ai_runtime_init(runtime);
    UmiDataServer *data = NULL;
    UmiAiWorkspace *workspace = NULL;
    UmiAiWorkspaceSourceChange *change = NULL;
    UmiStatus status = umi_data_server_create_memory(&data);
    if (status == UMI_STATUS_OK)
        status = UmiAiWorkspaceCreate(data, runtime, "source-lesson", &workspace);
    if (status == UMI_STATUS_OK)
        status = UmiAiWorkspacePutCollection(workspace, "workshop", "Workshop");
    if (status == UMI_STATUS_OK)
        status =
            UmiAiWorkspacePutSource(workspace, "notice.part.1", "workshop", "Notice", "Open at ten.", 1U);
    const char *ids[] = {"notice.part.1"};
    const char *text = "Open at noon.\nBring your notebook.";
    if (status == UMI_STATUS_OK)
        status = UmiAiWorkspaceSourceReplacementCreate(workspace, ids, 1U, "workshop", "Workshop", "notice",
                                                       "Notice", text, strlen(text), &change);
    UmiAiWorkspaceSourceChangeSummary summary = {0};
    if (status == UMI_STATUS_OK)
        status = UmiAiWorkspaceSourceChangeInspect(change, &summary);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < summary.beforeCount; ++i)
    {
        UmiAiWorkspaceSource source;
        status = UmiAiWorkspaceSourceChangeBeforeAt(change, i, &source);
        if (status == UMI_STATUS_OK)
            printf("BEFORE %s: %s\n", source.id, source.text);
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < summary.afterCount; ++i)
    {
        UmiAiWorkspaceSource source;
        status = UmiAiWorkspaceSourceChangeAfterAt(change, i, &source);
        if (status == UMI_STATUS_OK)
            printf("AFTER %s: %s\n", source.id, source.text);
    }
    /* This fixed lesson deliberately approves its own sample. An interactive
     * host must obtain the user's decision after displaying every passage,
     * and recreate the capture if the workspace changes during review. */
    if (status == UMI_STATUS_OK)
        status = UmiAiWorkspaceSourceChangeApply(workspace, change, true);
    if (status == UMI_STATUS_OK)
        puts("Reviewed source replacement saved. Earlier job evidence is retained.");
    else
        fprintf(stderr, "Source replacement could not complete (status %d).\n", (int)status);
    UmiAiWorkspaceSourceChangeDestroy(change);
    UmiAiWorkspaceDestroy(workspace);
    umi_data_server_destroy(data);
    umi_ai_runtime_destroy(runtime);
    free(runtime);
    return status == UMI_STATUS_OK ? 0 : 1;
}
