/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_source_change.c
 * PURPOSE: Exercise atomic source maintenance, preserved evidence, search changes and stale review rejection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/source_change.h"
#include "umicom/ai_workspace/providers.h"
#include "umicom/ai_workspace/evidence.h"
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
static UmiStatus Replace(UmiAiWorkspace *workspace, const char *const *ids, size_t count, const char *text,
                         UmiAiWorkspaceSourceChange **out)
{
    return UmiAiWorkspaceSourceReplacementCreate(workspace, ids, count, "workshop", "Workshop", "notice",
                                                 "Notice", text, strlen(text), out);
}
static bool HasText(UmiAiWorkspace *workspace, const char *id, const char *text)
{
    UmiAiWorkspaceSnapshot snapshot;
    if (UmiAiWorkspaceSnapshotRead(workspace, &snapshot) != UMI_STATUS_OK)
        return false;
    for (size_t i = 0U; i < snapshot.sourceCount; ++i)
    {
        UmiAiWorkspaceSource source;
        if (UmiAiWorkspaceSourceAt(workspace, i, &source) == UMI_STATUS_OK && strcmp(source.id, id) == 0)
            return strcmp(source.text, text) == 0;
    }
    return false;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    const char *cases[] = {"preview",        "replace",
                           "remove",         "owned",
                           "multiple",       "grow",
                           "collision",      "missing",
                           "duplicate",      "empty-selection",
                           "invalid-text",   "arguments",
                           "approval",       "repeat",
                           "stale",          "external-writer",
                           "other-owner",    "outer-transaction",
                           "noop",           "noop-external",
                           "new-collection", "collection-name",
                           "full-capacity",  "insufficient-capacity",
                           "overflow",       "busy",
                           "recovery",       "embedding",
                           "noop-embedding", "frozen-replace",
                           "frozen-remove",  "reload",
                           "sqlite-reload",  "sqlite-rollback"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    int failed = 0;
    UmiDataServer *data = NULL;
    UmiAiWorkspace *workspace = NULL, *other = NULL;
    UmiAiRuntime *runtime = calloc(1U, sizeof(*runtime));
    UmiAiWorkspaceSourceChange *change = NULL;
    UmiAiWorkspaceJob *job = NULL;
    char *large = NULL;
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
    OK(UmiAiWorkspaceCreate(data, runtime, "source-change", &workspace));
    OK(UmiAiWorkspacePutCollection(workspace, "workshop", "Workshop"));
    OK(UmiAiWorkspacePutSource(workspace, "notice.part.1", "workshop", "Notice", "Workshop opens at ten.\n",
                               1U));
    OK(UmiAiWorkspacePutSource(workspace, "notice.part.2", "workshop", "Notice", "Bring a notebook.", 2U));
    OK(UmiAiWorkspacePutSource(workspace, "unrelated", "workshop", "Other", "Unrelated passage", 7U));
    UmiAiWorkspaceSnapshot before, after;
    OK(UmiAiWorkspaceSnapshotRead(workspace, &before));
    const char *ids[] = {"notice.part.1", "notice.part.2"};
    size_t count = 2U;
    const char *replacement = "Workshop now opens at noon.\nBring water.";
    UmiAiWorkspaceSource source;
    UmiAiWorkspaceSourceChangeSummary summary;
    if (strcmp(name, "arguments") == 0)
    {
        CHECK(UmiAiWorkspaceSourceRemovalCreate(NULL, ids, 2U, &change) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiWorkspaceSourceRemovalCreate(workspace, ids, 2U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiWorkspaceSourceChangeInspect(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiWorkspaceSourceChangeBeforeAt(NULL, 0U, &source) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiWorkspaceSourceChangeAfterAt(NULL, 0U, &source) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiWorkspaceSourceChangeApply(workspace, NULL, true) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiWorkspaceSourceRemovalCreate(workspace, ids, UMI_AI_WORKSPACE_MAX_SOURCES + 1U, &change) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        goto unchanged;
    }
    if (strcmp(name, "invalid-text") == 0)
    {
        const char *bad[] = {"", "\xc0\xaf", "\xed\xa0\x80", "x\x01"};
        for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i)
            CHECK(Replace(workspace, ids, count, bad[i], &change) == UMI_STATUS_INVALID_ARGUMENT &&
                  change == NULL);
        CHECK(UmiAiWorkspaceSourceReplacementCreate(workspace, ids, count, "workshop", "Workshop", "notice",
                                                    "Notice", "a\0b", 3U,
                                                    &change) == UMI_STATUS_INVALID_ARGUMENT);
        goto unchanged;
    }
    if (strcmp(name, "missing") == 0 || strcmp(name, "duplicate") == 0 ||
        strcmp(name, "empty-selection") == 0)
    {
        if (strcmp(name, "missing") == 0)
            ids[1] = "missing";
        if (strcmp(name, "duplicate") == 0)
            ids[1] = ids[0];
        if (strcmp(name, "empty-selection") == 0)
            count = 0U;
        UmiStatus expected = count == 0U                    ? UMI_STATUS_INVALID_ARGUMENT
                             : strcmp(name, "missing") == 0 ? UMI_STATUS_NOT_FOUND
                                                            : UMI_STATUS_ALREADY_EXISTS;
        CHECK(Replace(workspace, ids, count, replacement, &change) == expected && change == NULL);
        CHECK(UmiAiWorkspaceSourceRemovalCreate(workspace, ids, count, &change) == expected &&
              change == NULL);
        goto unchanged;
    }
    if (strcmp(name, "overflow") == 0 || strcmp(name, "busy") == 0 || strcmp(name, "recovery") == 0)
    {
        uint64_t saved = workspace->state->revision;
        if (strcmp(name, "overflow") == 0)
            workspace->state->revision = UINT64_MAX;
        if (strcmp(name, "busy") == 0)
            workspace->busy = true;
        if (strcmp(name, "recovery") == 0)
            workspace->recoveryRequired = true;
        UmiStatus expected = strcmp(name, "overflow") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED
                             : strcmp(name, "busy") == 0   ? UMI_STATUS_BUSY
                                                           : UMI_STATUS_INVALID_STATE;
        CHECK(Replace(workspace, ids, count, replacement, &change) == expected && change == NULL);
        CHECK(UmiAiWorkspaceSourceRemovalCreate(workspace, ids, count, &change) == expected &&
              change == NULL);
        workspace->state->revision = saved;
        workspace->busy = false;
        workspace->recoveryRequired = false;
        goto unchanged;
    }
    if (strcmp(name, "full-capacity") == 0 || strcmp(name, "insufficient-capacity") == 0)
    {
        for (size_t i = 3U; i < UMI_AI_WORKSPACE_MAX_SOURCES; ++i)
        {
            char id[32];
            (void)snprintf(id, sizeof(id), "retained.%zu", i);
            OK(UmiAiWorkspacePutSource(workspace, id, "workshop", "Retained", "Other source", 1U));
        }
        OK(UmiAiWorkspaceSnapshotRead(workspace, &before));
        if (strcmp(name, "insufficient-capacity") == 0)
        {
            large = malloc(3201U);
            CHECK(large != NULL);
            memset(large, 'a', 3200U);
            large[3200] = '\0';
            CHECK(Replace(workspace, ids, count, large, &change) == UMI_STATUS_CAPACITY_EXCEEDED &&
                  change == NULL);
            goto unchanged;
        }
    }
    if (strcmp(name, "collision") == 0)
    {
        count = 1U;
        large = malloc(1601U);
        CHECK(large != NULL);
        memset(large, 'x', 1600U);
        large[1600] = '\0';
        CHECK(Replace(workspace, ids, count, large, &change) == UMI_STATUS_ALREADY_EXISTS && change == NULL);
        goto unchanged;
    }
    if (strcmp(name, "collection-name") == 0)
    {
        CHECK(UmiAiWorkspaceSourceReplacementCreate(workspace, ids, count, "workshop", "Wrong name", "notice",
                                                    "Notice", replacement, strlen(replacement),
                                                    &change) == UMI_STATUS_ALREADY_EXISTS);
        goto unchanged;
    }
    bool frozen = strncmp(name, "frozen-", 7U) == 0;
    if (frozen)
    {
        UmiAiProvider provider = {0};
        OK(UmiAiWorkspaceExtractiveProviderCreate(&provider));
        OK(umi_ai_provider_registry_add(&runtime->providers, &provider));
        OK(UmiAiWorkspacePrepare(workspace, "question", UMI_AI_WORKSPACE_GROUNDED_DRAFT,
                                 "umicom.extractive-preview", "extractive-preview", "workshop",
                                 "When does workshop open?", "writer", 512U));
        OK(UmiAiWorkspaceReview(workspace, "question", "reviewer", true));
        OK(UmiAiWorkspaceRun(workspace, "question", NULL));
        job = calloc(1U, sizeof(*job));
        CHECK(job != NULL);
        OK(UmiAiWorkspaceJobFind(workspace, "question", job));
        CHECK(job->state == UMI_AI_WORKSPACE_SUCCEEDED && job->evidenceCount == 1U);
        OK(UmiAiWorkspaceSnapshotRead(workspace, &before));
    }
    bool noop = strncmp(name, "noop", 4U) == 0;
    if (noop)
    {
        count = 1U;
        replacement = "Workshop opens at ten.\n";
    }
    if (strcmp(name, "grow") == 0)
    {
        large = malloc(3201U);
        CHECK(large != NULL);
        memset(large, 'x', 3200U);
        large[3200] = '\0';
        replacement = large;
    }
    if (strcmp(name, "multiple") == 0)
    {
        ids[0] = "notice.part.2";
        ids[1] = "notice.part.1";
    }
    bool removal = strcmp(name, "remove") == 0 || strcmp(name, "frozen-remove") == 0;
    if (removal)
        OK(UmiAiWorkspaceSourceRemovalCreate(workspace, ids, count, &change));
    else if (strcmp(name, "new-collection") == 0)
        OK(UmiAiWorkspaceSourceReplacementCreate(workspace, ids, count, "archive", "Archive", "notice",
                                                 "Notice", replacement, strlen(replacement), &change));
    else
        OK(Replace(workspace, ids, count, replacement, &change));
    OK(UmiAiWorkspaceSourceChangeInspect(change, &summary));
    CHECK(summary.beforeCount == count && summary.workspaceRevision == before.revision && !summary.applied);
    CHECK(summary.hasChanges != noop);
    CHECK(summary.afterCount == (removal ? 0U : strcmp(name, "grow") == 0 ? 3U : 1U));
    OK(UmiAiWorkspaceSourceChangeBeforeAt(change, 0U, &source));
    CHECK(strcmp(source.id, ids[0]) == 0);
    CHECK(UmiAiWorkspaceSourceChangeBeforeAt(change, count, &source) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiAiWorkspaceSourceChangeAfterAt(change, summary.afterCount, &source) == UMI_STATUS_NOT_FOUND);
    OK(UmiAiWorkspaceSourceChangeCheck(workspace, change));
    OK(UmiAiWorkspaceSourceChangeInspect(change, &summary));
    CHECK(!summary.applied);
    if (strcmp(name, "preview") == 0)
        goto unchanged;
    if (strcmp(name, "owned") == 0)
    {
        /* Inspected records are copies. A caller cannot rewrite the approved plan. */
        OK(UmiAiWorkspaceSourceChangeAfterAt(change, 0U, &source));
        strcpy(source.text, "Unapproved edit");
        OK(UmiAiWorkspaceSourceChangeAfterAt(change, 0U, &source));
        CHECK(strcmp(source.text, replacement) == 0);
    }
    if (strcmp(name, "approval") == 0)
    {
        CHECK(UmiAiWorkspaceSourceChangeApply(workspace, change, false) == UMI_STATUS_PERMISSION_DENIED);
        OK(UmiAiWorkspaceSourceChangeInspect(change, &summary));
        CHECK(!summary.applied);
        goto unchanged;
    }
    if (strcmp(name, "stale") == 0 || strcmp(name, "external-writer") == 0 ||
        strcmp(name, "noop-external") == 0 || strcmp(name, "other-owner") == 0)
    {
        OK(UmiAiWorkspaceCreate(data, runtime, "source-change", &other));
        if (strcmp(name, "other-owner") == 0)
        {
            CHECK(UmiAiWorkspaceSourceChangeApply(other, change, true) == UMI_STATUS_INVALID_ARGUMENT);
            goto unchanged;
        }
        OK(UmiAiWorkspacePutCollection(strcmp(name, "stale") == 0 ? workspace : other, "new", "New"));
        OK(UmiAiWorkspaceSnapshotRead(workspace, &before));
        CHECK(UmiAiWorkspaceSourceChangeApply(workspace, change, true) == UMI_STATUS_BUSY);
        goto unchanged;
    }
    if (strcmp(name, "outer-transaction") == 0)
    {
        OK(umi_data_server_begin(data));
        CHECK(UmiAiWorkspaceSourceChangeApply(workspace, change, true) == UMI_STATUS_BUSY);
        CHECK(umi_data_server_in_transaction(data));
        OK(umi_data_server_rollback(data));
        goto unchanged;
    }
    if (strcmp(name, "sqlite-rollback") == 0)
    {
        size_t records = umi_data_server_count(data);
/* Sources are stored as separate metadata and text records. Inject failure on the real text key after preceding writes, so the unchanged-state checks exercise rollback; retain the obsolete trigger for review. */
#if 0
        OK(umi_data_server_execute(data, "CREATE TRIGGER fail_source_change BEFORE INSERT ON umicom_kv WHEN "
                                         "NEW.key LIKE '%/s/1' BEGIN SELECT RAISE(ABORT,'injected'); END;"));
#endif
        OK(umi_data_server_execute(data, "CREATE TRIGGER fail_source_change BEFORE INSERT ON umicom_kv WHEN "
                                         "NEW.key LIKE '%/s/1/t' BEGIN SELECT RAISE(ABORT,'injected'); END;"));
        CHECK(UmiAiWorkspaceSourceChangeApply(workspace, change, true) != UMI_STATUS_OK);
        CHECK(umi_data_server_count(data) == records);
        OK(UmiAiWorkspaceReload(workspace));
        goto unchanged;
    }
    bool embedding = strcmp(name, "embedding") == 0 || strcmp(name, "noop-embedding") == 0;
    if (embedding)
    {
        UmiAiWorkspaceSource saved;
        OK(UmiAiWorkspaceSourceAt(workspace, 0U, &saved));
        UmiAiEmbedding value = {0};
        value.dimension = 2U;
        value.values[0] = 1.0F;
        OK(UmiAiWorkspaceSetEmbedding(workspace, saved.id, saved.revision, "fixture-vector", &value));
    }
    OK(UmiAiWorkspaceSourceChangeApply(workspace, change, true));
    OK(UmiAiWorkspaceSnapshotRead(workspace, &after));
    CHECK(after.sourceCount == before.sourceCount - count + summary.afterCount);
    CHECK(after.revision == before.revision + (noop ? 0U : 1U) &&
          after.corpusRevision == before.corpusRevision + (noop ? 0U : 1U));
    CHECK(HasText(workspace, "unrelated", "Unrelated passage"));
    if (!removal && strcmp(name, "grow") != 0)
        CHECK(HasText(workspace, "notice.part.1", replacement));
    if (strcmp(name, "grow") == 0)
    {
        size_t offset = 0U;
        for (size_t i = 0U; i < summary.afterCount; ++i)
        {
            OK(UmiAiWorkspaceSourceChangeAfterAt(change, i, &source));
            CHECK(memcmp(source.text, large + offset, strlen(source.text)) == 0);
            CHECK(HasText(workspace, source.id, source.text));
            offset += strlen(source.text);
        }
        CHECK(offset == 3200U);
    }
    if (strcmp(name, "new-collection") == 0)
        CHECK(after.collectionCount == before.collectionCount + 1U);
    if (strcmp(name, "repeat") == 0)
        CHECK(UmiAiWorkspaceSourceChangeApply(workspace, change, true) == UMI_STATUS_INVALID_STATE);
    if (embedding)
    {
        UmiAiWorkspaceEvidence results[4];
        size_t matches = 0U;
        UmiAiEmbedding value = {0};
        value.dimension = 2U;
        value.values[0] = 1.0F;
        status = UmiAiWorkspaceSearch(workspace, "workshop", "unmatched", "fixture-vector", &value, NULL,
                                      results, 4U, &matches);
        CHECK(noop ? status == UMI_STATUS_OK && matches == 1U : status == UMI_STATUS_UNAVAILABLE);
    }
    if (frozen)
    {
        UmiAiWorkspaceJob *saved = calloc(1U, sizeof(*saved));
        CHECK(saved != NULL);
        status = UmiAiWorkspaceJobFind(workspace, "question", saved);
        bool intact = status == UMI_STATUS_OK && strcmp(saved->response.text, job->response.text) == 0 &&
                      strcmp(saved->evidence[0].source.text, job->evidence[0].source.text) == 0 &&
                      saved->corpusRevision == job->corpusRevision;
        free(saved);
        CHECK(intact);
        UmiAiWorkspaceEvidence results[4];
        size_t matches = 0U;
        OK(UmiAiWorkspaceSearch(workspace, "workshop", "workshop", NULL, NULL, NULL, results, 4U, &matches));
        CHECK(removal ? matches == 0U : matches == 1U && strstr(results[0].source.text, "noon") != NULL);
    }
    if (strcmp(name, "reload") == 0 || strcmp(name, "sqlite-reload") == 0)
    {
        UmiAiWorkspaceDestroy(workspace);
        workspace = NULL;
        OK(UmiAiWorkspaceCreate(data, runtime, "source-change", &workspace));
        OK(UmiAiWorkspaceSnapshotRead(workspace, &after));
        CHECK(after.sourceCount == 2U && HasText(workspace, "notice.part.1", replacement));
    }
    goto cleanup;
unchanged:
    OK(UmiAiWorkspaceSnapshotRead(workspace, &after));
    CHECK(after.revision == before.revision && after.corpusRevision == before.corpusRevision &&
          after.sourceCount == before.sourceCount);
    CHECK(HasText(workspace, "notice.part.1", "Workshop opens at ten.\n") &&
          HasText(workspace, "notice.part.2", "Bring a notebook."));
cleanup:
    free(job);
    free(large);
    UmiAiWorkspaceSourceChangeDestroy(change);
    UmiAiWorkspaceDestroy(other);
    UmiAiWorkspaceDestroy(workspace);
    umi_data_server_destroy(data);
    if (runtime != NULL)
    {
        umi_ai_runtime_destroy(runtime);
        free(runtime);
    }
    return failed;
}
