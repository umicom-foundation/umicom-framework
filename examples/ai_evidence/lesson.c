/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/ai_evidence/lesson.c
 * PURPOSE: Follow selected Notes passages through review, generation and source change.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "lesson.h"
#include "umicom/ai_workspace/providers.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiAiEvidenceLesson(FILE *output, bool verbose, bool local, uint16_t port, const char *model)
{
    UmiDataServer *data = NULL; UmiAiWorkspace *workspace = NULL;
    UmiAiWorkspaceCancellation *cancellation = NULL; UmiAiEvidenceReview *review = NULL;
    UmiAiRuntime *runtime = calloc(1U, sizeof *runtime);
    UmiAiWorkspaceEvidence *selected = calloc(UMI_AI_WORKSPACE_MAX_EVIDENCE, sizeof *selected);
    char *report = calloc(UMI_AI_EVIDENCE_REPORT_CAPACITY, 1U);
    UmiStatus status = UMI_STATUS_OUT_OF_MEMORY;
    if (runtime == NULL || selected == NULL || report == NULL) goto cleanup;
    umi_ai_runtime_init(runtime);
#define STEP(expression) do { status = (expression); if (status != UMI_STATUS_OK) goto cleanup; } while (0)
    STEP(umi_data_server_create_memory(&data));
    STEP(UmiAiWorkspaceCancellationCreate(&cancellation));
    UmiAiProvider provider = {0};
    STEP(local ? UmiAiWorkspaceLocalProviderCreate(port, 10000U, cancellation, &provider) :
        UmiAiWorkspaceExtractiveProviderCreate(&provider));
    status = umi_ai_provider_registry_add(&runtime->providers, &provider);
    if (status != UMI_STATUS_OK) {
        if (provider.destroy != NULL) provider.destroy(provider.instance);
        goto cleanup;
    }
    STEP(UmiAiWorkspaceCreate(data, runtime, "notes.lesson", &workspace));
    STEP(UmiAiWorkspacePutCollection(workspace, "notes", "Umicom Notes handbook"));
    STEP(UmiAiWorkspacePutSource(workspace, "save", "notes", "Saving a note",
        "Save checkpoint commits the current note.\nClosing without saving discards the draft.\n", 10U));
    STEP(UmiAiWorkspacePutSource(workspace, "review", "notes", "Checking a draft",
        "Read the source passage before using a draft.\nA citation alone does not prove the answer is correct.\n", 30U));
    UmiAiWorkspaceSnapshot snapshot;
    STEP(UmiAiWorkspaceSnapshotRead(workspace, &snapshot));
    size_t count = 0U;
    STEP(UmiAiWorkspaceSearch(workspace, "notes", "saving draft", NULL, NULL, NULL,
        selected, UMI_AI_WORKSPACE_MAX_EVIDENCE, &count));
    if (count != 2U) { status = UMI_STATUS_INTERNAL_ERROR; goto cleanup; }
    const char *providerId = local ? "umicom.local-chat" : "umicom.extractive-preview";
    const char *modelId = local ? model : "extractive-preview";
    STEP(UmiAiWorkspacePrepareEvidence(workspace, "lesson", providerId, modelId,
        "notes", "Explain how to save and check a note. Cite the supplied sources.", "writer",
        256U, snapshot.corpusRevision, selected, count));
    STEP(UmiAiWorkspaceReview(workspace, "lesson", "reviewer", true));
    STEP(UmiAiWorkspaceRun(workspace, "lesson", cancellation));
    STEP(UmiAiEvidenceCapture(workspace, "lesson", &review));
    UmiAiEvidenceSummary summary;
    STEP(UmiAiEvidenceSummaryRead(review, &summary));
    if (!summary.hasResponse || !summary.referenceBindingsValid) { status = UMI_STATUS_INTERNAL_ERROR; goto cleanup; }
    if (verbose) { STEP(UmiAiEvidenceFormat(review, report, UMI_AI_EVIDENCE_REPORT_CAPACITY, NULL)); fputs(report, output); }
    fprintf(output, "Selected passages: %zu. Reference targets exist; prose still needs checking.\n", summary.sourceCount);
    UmiAiEvidenceDestroy(review); review = NULL;
    STEP(UmiAiWorkspacePutSource(workspace, "save", "notes", "Saving a note",
        "The current handbook has been amended; inspect the saved source version.\n", 10U));
    STEP(UmiAiEvidenceCapture(workspace, "lesson", &review));
    STEP(UmiAiEvidenceSummaryRead(review, &summary));
    if (summary.changedSources != 1U) { status = UMI_STATUS_INTERNAL_ERROR; goto cleanup; }
    /* Reports own their copies. Close all services before demonstrating that
     * an old citation still resolves to its original text. */
    UmiAiWorkspaceDestroy(workspace); workspace = NULL;
    umi_data_server_destroy(data); data = NULL;
    for (size_t i = 0U; i < summary.sourceCount; ++i) {
        UmiAiEvidenceSourceCheck source;
        STEP(UmiAiEvidenceSourceAt(review, i, &source));
        if (strcmp(source.frozen.id, "save") == 0) {
            char line[160]; STEP(UmiAiEvidenceCopyLines(review, i, 10U, 10U, line, sizeof line, NULL));
            if (strcmp(line, "Save checkpoint commits the current note.\n") != 0) { status = UMI_STATUS_INTERNAL_ERROR; goto cleanup; }
        }
    }
    fputs("One source changed. The saved citation still resolves to the frozen passage.\n", output);
    fputs(local ? "Practice complete. One explicitly requested local model call; no tools or source files were executed.\n" :
        "Practice complete. Extractive preview, not model inference; no network, files or tools were used.\n", output);
cleanup:
    UmiAiEvidenceDestroy(review); UmiAiWorkspaceDestroy(workspace);
    umi_data_server_destroy(data);
    if (runtime != NULL) { umi_ai_runtime_destroy(runtime); free(runtime); }
    UmiAiWorkspaceCancellationDestroy(cancellation); free(selected); free(report);
    return status;
#undef STEP
}
