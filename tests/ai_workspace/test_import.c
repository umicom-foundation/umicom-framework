/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_import.c
 * PURPOSE: Exercise complete imported text, transaction rollback, stale captures and source-grounded retrieval.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/import.h"
#include "umicom/ai_workspace/providers.h"
#include "workspace_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                                  \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
#define OK(c) CHECK((c) == UMI_STATUS_OK)
static UmiStatus Prepare(UmiAiWorkspace *workspace, const char *text, size_t bytes,
                         UmiAiWorkspaceImport **out)
{
    return UmiAiWorkspaceImportCreate(workspace, "documents", "Documents", "notice", "Opening notice", text,
                                      bytes, out);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    const char *cases[] = {"preview",
                           "apply",
                           "owned",
                           "unicode",
                           "normalization",
                           "long-line",
                           "lines",
                           "approval",
                           "repeat",
                           "collision",
                           "collection-title",
                           "stale",
                           "external-writer",
                           "capacity",
                           "collections-full",
                           "limits",
                           "invalid",
                           "arguments",
                           "reload",
                           "search",
                           "grounded",
                           "sqlite-rollback",
                           "sqlite-reload",
                           "outer-transaction",
                           "other-owner",
                           "revision-overflow",
                           "maximum",
                           "partial-capacity",
                           "late-collision"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    int failed = 0;
    UmiDataServer *data = NULL;
    UmiAiWorkspace *workspace = NULL, *other = NULL;
    UmiAiWorkspaceImport *import = NULL, *second = NULL;
    UmiAiRuntime *runtime = calloc(1U, sizeof(*runtime));
    char *large = NULL;
    UmiAiWorkspaceJob *job = NULL;
    CHECK(runtime != NULL);
    umi_ai_runtime_init(runtime);
    bool sqlite = strncmp(name, "sqlite-", 7U) == 0;
    UmiStatus status =
        sqlite ? umi_data_server_create_sqlite(":memory:", &data) : umi_data_server_create_memory(&data);
    if (sqlite && status == UMI_STATUS_UNAVAILABLE)
    {
        failed = 77;
        goto cleanup;
    }
    OK(status);
    OK(UmiAiWorkspaceCreate(data, runtime, "import-test", &workspace));
    UmiAiWorkspaceSnapshot before, after;
    OK(UmiAiWorkspaceSnapshotRead(workspace, &before));
    const char *text = "The workshop opens at ten.\nBring your notebook.\n";
    size_t length = strlen(text);
    if (strcmp(name, "arguments") == 0)
    {
        CHECK(Prepare(NULL, text, length, &import) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(Prepare(workspace, text, length, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiWorkspaceImportInspect(NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiWorkspaceImportApply(workspace, NULL, true) == UMI_STATUS_INVALID_ARGUMENT);
        goto unchanged;
    }
    if (strcmp(name, "limits") == 0)
    {
        CHECK(Prepare(workspace, "x", UMI_AI_WORKSPACE_IMPORT_MAX_BYTES + 1U, &import) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        char identifier[57];
        memset(identifier, 'a', sizeof(identifier));
        identifier[56] = '\0';
        CHECK(UmiAiWorkspaceImportCreate(workspace, "documents", "Documents", identifier, "Title", text,
                                         length, &import) == UMI_STATUS_INVALID_ARGUMENT);
        goto unchanged;
    }
    if (strcmp(name, "invalid") == 0)
    {
        const char *bad[] = {"", "\xef\xbb\xbf", "a\x01", "\xc0\xaf", "\xed\xa0\x80", "\xf4\x90\x80\x80"};
        for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i)
            CHECK(Prepare(workspace, bad[i], strlen(bad[i]), &import) == UMI_STATUS_INVALID_ARGUMENT &&
                  import == NULL);
        CHECK(Prepare(workspace, "a\0b", 3U, &import) == UMI_STATUS_INVALID_ARGUMENT);
        goto unchanged;
    }
    if (strcmp(name, "partial-capacity") == 0 || strcmp(name, "late-collision") == 0)
    {
        OK(UmiAiWorkspacePutCollection(workspace, "existing", "Existing"));
        if (strcmp(name, "partial-capacity") == 0)
        {
            for (unsigned i = 0U; i < UMI_AI_WORKSPACE_MAX_SOURCES - 1U; ++i)
            {
                char id[32];
                (void)snprintf(id, sizeof(id), "saved.%u", i);
                OK(UmiAiWorkspacePutSource(workspace, id, "existing", "Saved", "Retained", 1U));
            }
        }
        else
            OK(UmiAiWorkspacePutSource(workspace, "notice.part.2", "existing", "Unrelated", "Retained", 1U));
        large = malloc(3001U);
        CHECK(large != NULL);
        memset(large, 'x', 3000U);
        large[3000] = '\0';
        OK(UmiAiWorkspaceSnapshotRead(workspace, &before));
        CHECK(Prepare(workspace, large, 3000U, &import) == (strcmp(name, "partial-capacity") == 0
                                                                ? UMI_STATUS_CAPACITY_EXCEEDED
                                                                : UMI_STATUS_ALREADY_EXISTS));
        CHECK(import == NULL);
        goto unchanged;
    }
    if (strcmp(name, "capacity") == 0 || strcmp(name, "collections-full") == 0)
    {
        if (strcmp(name, "capacity") == 0)
        {
            OK(UmiAiWorkspacePutCollection(workspace, "other", "Other"));
            for (unsigned i = 0U; i < UMI_AI_WORKSPACE_MAX_SOURCES; ++i)
            {
                char id[32];
                (void)snprintf(id, sizeof(id), "saved.%u", i);
                OK(UmiAiWorkspacePutSource(workspace, id, "other", "Existing", "Retained", 1U));
            }
        }
        else
            for (unsigned i = 0U; i < UMI_AI_WORKSPACE_MAX_COLLECTIONS; ++i)
            {
                char id[32];
                (void)snprintf(id, sizeof(id), "saved.%u", i);
                OK(UmiAiWorkspacePutCollection(workspace, id, "Existing"));
            }
        OK(UmiAiWorkspaceSnapshotRead(workspace, &before));
        CHECK(Prepare(workspace, text, length, &import) == UMI_STATUS_CAPACITY_EXCEEDED);
        goto unchanged;
    }
    if (strcmp(name, "collision") == 0 || strcmp(name, "collection-title") == 0)
    {
        OK(UmiAiWorkspacePutCollection(
            workspace, "documents", strcmp(name, "collection-title") == 0 ? "Different name" : "Documents"));
        if (strcmp(name, "collision") == 0)
            OK(UmiAiWorkspacePutSource(workspace, "notice.part.1", "documents", "Existing", "Do not replace",
                                       1U));
        OK(UmiAiWorkspaceSnapshotRead(workspace, &before));
        CHECK(Prepare(workspace, text, length, &import) == UMI_STATUS_ALREADY_EXISTS);
        goto unchanged;
    }
    if (strcmp(name, "revision-overflow") == 0)
    {
        workspace->state->revision = UINT64_MAX;
        CHECK(Prepare(workspace, text, length, &import) == UMI_STATUS_CAPACITY_EXCEEDED);
        workspace->state->revision = 0U;
        workspace->state->corpusRevision = UINT64_MAX;
        CHECK(Prepare(workspace, text, length, &import) == UMI_STATUS_CAPACITY_EXCEEDED);
        goto cleanup;
    }
    if (strcmp(name, "normalization") == 0)
    {
        text = "\xef\xbb\xbf"
               "one\r\ntwo\rthree\n";
        length = strlen(text);
    }
    if (strcmp(name, "unicode") == 0)
    {
        text = "Caf\xc3\xa9\n\xf0\x9f\x8e\xb5";
        length = strlen(text);
    }
    if (strcmp(name, "long-line") == 0 || strcmp(name, "lines") == 0 || strcmp(name, "owned") == 0)
    {
        length = 5000U;
        large = malloc(length + 1U);
        CHECK(large != NULL);
        memset(large, 'x', length);
        large[length] = '\0';
        if (strcmp(name, "long-line") == 0)
            memcpy(large + 1534U, "\xe2\x82\xac", 3U);
        if (strcmp(name, "lines") == 0)
            for (size_t i = 79U; i < length; i += 80U)
                large[i] = '\n';
        text = large;
    }
    if (strcmp(name, "maximum") == 0 || strcmp(name, "sqlite-rollback") == 0)
    {
        length = strcmp(name, "maximum") == 0 ? UMI_AI_WORKSPACE_IMPORT_MAX_BYTES : 3000U;
        large = malloc(length + 1U);
        CHECK(large != NULL);
        memset(large, 'x', length);
        large[length] = '\0';
        text = large;
    }
    OK(Prepare(workspace, text, length, &import));
    UmiAiWorkspaceImportSummary summary;
    OK(UmiAiWorkspaceImportInspect(import, &summary));
    CHECK(summary.inputBytes == length && summary.passageCount > 0U && summary.createsCollection &&
          !summary.saved);
    if (strcmp(name, "owned") == 0)
        large[0] = 'q';
    size_t offset = 0U;
    uint32_t line = 1U;
    for (size_t i = 0U; i < summary.passageCount; ++i)
    {
        UmiAiWorkspaceImportPassage passage;
        OK(UmiAiWorkspaceImportPassageAt(import, i, &passage));
        CHECK(passage.textOffset == offset && passage.source.firstLine == line &&
              passage.byteCount == strlen(passage.source.text));
        CHECK(AwTextValid(passage.source.text, sizeof(passage.source.text), false));
        if (strcmp(name, "normalization") == 0)
            CHECK(strcmp(passage.source.text, "one\ntwo\nthree\n") == 0 && passage.source.lastLine == 3U);
        else if (strcmp(name, "owned") == 0 && i == 0U)
            CHECK(passage.source.text[0] == 'x');
        else
            CHECK(memcmp(passage.source.text, text + offset, passage.byteCount) == 0);
        for (size_t j = 0U; j < passage.byteCount; ++j)
            if (passage.source.text[j] == '\n')
                ++line;
        offset += passage.byteCount;
    }
    CHECK(offset == summary.textBytes);
    if (strcmp(name, "preview") == 0)
        goto unchanged;
    if (strcmp(name, "approval") == 0)
    {
        CHECK(UmiAiWorkspaceImportApply(workspace, import, false) == UMI_STATUS_PERMISSION_DENIED);
        goto unchanged;
    }
    if (strcmp(name, "stale") == 0 || strcmp(name, "external-writer") == 0 ||
        strcmp(name, "other-owner") == 0)
    {
        OK(UmiAiWorkspaceCreate(data, runtime, "import-test", &other));
        if (strcmp(name, "other-owner") == 0)
        {
            CHECK(UmiAiWorkspaceImportApply(other, import, true) == UMI_STATUS_INVALID_ARGUMENT);
            goto unchanged;
        }
        OK(UmiAiWorkspacePutCollection(strcmp(name, "stale") == 0 ? workspace : other, "changed", "Changed"));
        OK(UmiAiWorkspaceSnapshotRead(workspace, &before));
        CHECK(UmiAiWorkspaceImportApply(workspace, import, true) == UMI_STATUS_BUSY);
        goto unchanged;
    }
    if (strcmp(name, "outer-transaction") == 0)
    {
        OK(umi_data_server_begin(data));
        CHECK(UmiAiWorkspaceImportApply(workspace, import, true) == UMI_STATUS_BUSY);
        CHECK(umi_data_server_in_transaction(data));
        OK(umi_data_server_rollback(data));
        goto unchanged;
    }
    if (strcmp(name, "sqlite-rollback") == 0)
    {
        OK(umi_data_server_execute(data, "CREATE TRIGGER fail_import BEFORE INSERT ON umicom_kv WHEN NEW.key "
                                         "LIKE '%/s/0' BEGIN SELECT RAISE(ABORT,'injected'); END;"));
        CHECK(UmiAiWorkspaceImportApply(workspace, import, true) != UMI_STATUS_OK);
        CHECK(umi_data_server_count(data) == 0U);
        OK(UmiAiWorkspaceReload(workspace));
        goto unchanged;
    }
    OK(UmiAiWorkspaceImportApply(workspace, import, true));
    OK(UmiAiWorkspaceSnapshotRead(workspace, &after));
    CHECK(after.sourceCount == summary.passageCount && after.collectionCount == 1U &&
          after.revision == before.revision + 1U && after.corpusRevision == before.corpusRevision + 1U);
    if (strcmp(name, "repeat") == 0)
    {
        CHECK(UmiAiWorkspaceImportApply(workspace, import, true) == UMI_STATUS_INVALID_STATE);
        CHECK(Prepare(workspace, text, length, &second) == UMI_STATUS_ALREADY_EXISTS);
    }
    if (strcmp(name, "reload") == 0 || strcmp(name, "sqlite-reload") == 0)
    {
        UmiAiWorkspaceDestroy(workspace);
        workspace = NULL;
        OK(UmiAiWorkspaceCreate(data, runtime, "import-test", &workspace));
        OK(UmiAiWorkspaceSnapshotRead(workspace, &after));
        CHECK(after.sourceCount == summary.passageCount);
        UmiAiWorkspaceSource source;
        OK(UmiAiWorkspaceSourceAt(workspace, 0U, &source));
        CHECK(strcmp(source.text, text) == 0);
    }
    if (strcmp(name, "search") == 0 || strcmp(name, "grounded") == 0)
    {
        UmiAiWorkspaceEvidence results[4];
        size_t count = 0U;
        OK(UmiAiWorkspaceSearch(workspace, "documents", "workshop", NULL, NULL, NULL, results, 4U, &count));
        CHECK(count == 1U && strcmp(results[0].source.id, "notice.part.1") == 0);
        if (strcmp(name, "grounded") == 0)
        {
            UmiAiProvider provider = {0};
            OK(UmiAiWorkspaceExtractiveProviderCreate(&provider));
            OK(umi_ai_provider_registry_add(&runtime->providers, &provider));
            OK(UmiAiWorkspacePrepare(workspace, "question", UMI_AI_WORKSPACE_GROUNDED_DRAFT,
                                     "umicom.extractive-preview", "extractive-preview", "documents",
                                     "When does the workshop open?", "writer", 512U));
            job = calloc(1U, sizeof(*job));
            CHECK(job != NULL);
            OK(UmiAiWorkspaceJobFind(workspace, "question", job));
            CHECK(job->evidenceCount == 1U && strcmp(job->evidence[0].source.text, text) == 0);
            OK(UmiAiWorkspaceReview(workspace, "question", "reviewer", true));
            OK(UmiAiWorkspaceRun(workspace, "question", NULL));
            OK(UmiAiWorkspaceJobFind(workspace, "question", job));
            CHECK(job->state == UMI_AI_WORKSPACE_SUCCEEDED && strstr(job->response.text, "[S1]") != NULL);
        }
    }
    goto cleanup;
unchanged:
    OK(UmiAiWorkspaceSnapshotRead(workspace, &after));
    CHECK(after.revision == before.revision && after.corpusRevision == before.corpusRevision &&
          after.collectionCount == before.collectionCount && after.sourceCount == before.sourceCount);
cleanup:
    free(job);
    free(large);
    UmiAiWorkspaceImportDestroy(import);
    UmiAiWorkspaceImportDestroy(second);
    UmiAiWorkspaceDestroy(other);
    UmiAiWorkspaceDestroy(workspace);
    umi_data_server_destroy(data);
    if (runtime != NULL)
        umi_ai_runtime_destroy(runtime);
    free(runtime);
    return failed;
}
