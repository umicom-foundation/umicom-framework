/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_publication_execution.c
 *
 * PURPOSE:
 *   Exercise complete publication with in-memory Git remotes; no real repository or network is touched.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "publication_internal.h"
#include "umicom/platform/clock.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct FakeRepository {
    char path[UMI_PATH_CAPACITY];
    char head[65];
    char pin[65];
    int dirty;
    int staged;
    /* Recovery changes only references; the fixture keeps file state separate. */
    int detached;
    char local_main[65];
} FakeRepository;

typedef struct FakeWorkspace {
    char root[UMI_PATH_CAPACITY];
    FakeRepository repos[3]; /* primary, parent, duplicate child */
    char child_remote[65];
    char parent_remote[65];
    const char *scenario;
    int owns_fixture;
    size_t adds, commits, pushes, fetches, locks, merges;
    size_t advances, attachments;
    int sequence[32];
    size_t sequence_count;
} FakeWorkspace;

static void sha(char *out, char digit)
{
    memset(out, digit, 40U);
    out[40] = '\0';
}

static UmiStatus answer(UmiPublicationCapture *out, int code, const char *format, ...)
{
    char buffer[8192];
    va_list args;
    va_start(args, format);
    int written = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    if (written < 0 || (size_t)written >= sizeof(buffer)) return UMI_STATUS_CAPACITY_EXCEEDED;
    out->bytes = malloc((size_t)written + 1U);
    if (!out->bytes) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(out->bytes, buffer, (size_t)written + 1U);
    out->size = (size_t)written;
    out->exit_code = code;
    return UMI_STATUS_OK;
}

static int repository_index(FakeWorkspace *workspace, const char *path)
{
    for (int i = 0; i < 3; ++i)
        if (umi_path_equal(path, workspace->repos[i].path)) return i;
    return -1;
}

static UmiStatus fake_git(void *context, const char *path,
    const char *const *args, size_t count, UmiPublicationCapture *out)
{
    FakeWorkspace *workspace = context;
    int index = repository_index(workspace, path);
    if (index < 0 || count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    FakeRepository *repo = &workspace->repos[index];
    char *remote = index == 1 ? workspace->parent_remote : workspace->child_remote;
    const char *name = index == 1 ? "parent" : "child";
    int primary = index == 0;
    const char *scenario = workspace->scenario;

    if (strcmp(args[0], "ls-files") == 0) {
        if (count >= 2U && strcmp(args[1], "--stage") == 0) {
            if (primary && strcmp(scenario, "stage-conflict") == 0)
                return answer(out, 0, "100644 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 1\tsrc/main.c%c", 0);
            if (primary && strcmp(scenario, "malformed-index") == 0)
                return answer(out, 0, "100644 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 0\tsrc/main.c");
            if (primary && strcmp(scenario, "index-change") == 0 && workspace->adds)
                return answer(out, 0, "160000 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 0\tunexpected%c", 0);
            if (index == 1)
                return answer(out, 0, "160000 %s 0\tmodule%c", repo->pin, 0);
            return answer(out, 0, "100644 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 0\tsrc/main.c%c", 0);
        }
        if (primary && (strcmp(scenario, "private") == 0 ||
            (strcmp(scenario, "late-private") == 0 && workspace->adds)))
            return answer(out, 0, "UPDATE_private.md%c", 0);
        if (index == 1) return answer(out, 0, ".gitmodules%cmodule%c", 0, 0);
        return answer(out, 0, "src/main.c%c", 0);
    }
    if (strcmp(args[0], "rev-parse") == 0) {
        if (strcmp(args[1], "--show-toplevel") == 0) return answer(out, 0, "%s\n", path);
        if (strcmp(args[1], "--absolute-git-dir") == 0) return answer(out, 0, "%s/.git\n", path);
        if (strcmp(args[1], "HEAD^{tree}") == 0)
            return answer(out, 0, "%s\n", strcmp(scenario, "hook-tree") == 0 ?
                "ffffffffffffffffffffffffffffffffffffffff" :
                "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee");
        /* Preserve the earlier successful HEAD response while injecting a
         * probe failure after the fake pull has already completed. */
        if (count == 3U && strcmp(args[2], "HEAD") == 0 && index == 2 &&
            strcmp(scenario, "pull-head-read-failure") == 0 && workspace->merges)
            return answer(out, 1, "");
        if (count == 3U && strcmp(args[2], "HEAD") == 0) return answer(out, 0, "%s\n", repo->head);
        if (count == 3U && strcmp(args[2], "refs/heads/main") == 0) {
            if (strcmp(scenario, "attach-missing-main") == 0) return answer(out, 1, "");
            return answer(out, 0, "%s\n", repo->local_main);
        }
        return answer(out, 0, "%s\n", remote);
    }
/* The fake backend now models detached HEAD and compare-and-swap branch recovery independently from source files. The former implementation is retained for review. */
#if 0
    if (strcmp(args[0], "symbolic-ref") == 0) {
        if (primary && strcmp(scenario, "detached") == 0) return answer(out, 1, "");
        return answer(out, 0, "main\n");
    }
#endif
    if (strcmp(args[0], "symbolic-ref") == 0) {
        if (count == 5U && strcmp(args[1], "-m") == 0) {
            if (strcmp(args[3], "HEAD") != 0 || strcmp(args[4], "refs/heads/main") != 0 ||
                !repo->detached || strcmp(repo->head, repo->local_main) != 0)
                return UMI_STATUS_INVALID_ARGUMENT;
            if (strcmp(scenario, "attach-bind-failure") == 0) return answer(out, 1, "");
            repo->detached = 0;
            ++workspace->attachments;
            return answer(out, 0, "");
        }
        if (repo->detached || (primary && strcmp(scenario, "detached") == 0))
            return answer(out, 1, "");
        return answer(out, 0, "main\n");
    }
    if (strcmp(args[0], "merge-base") == 0) {
        if (count != 4U || strcmp(args[1], "--is-ancestor") != 0 ||
            strcmp(args[2], repo->local_main) != 0 || strcmp(args[3], repo->head) != 0)
            return UMI_STATUS_INVALID_ARGUMENT;
        return answer(out, strcmp(scenario, "attach-divergent-main") == 0 ? 1 : 0, "");
    }
    if (strcmp(args[0], "worktree") == 0) {
        if (count != 4U || strcmp(args[1], "list") != 0 ||
            strcmp(args[2], "--porcelain") != 0 || strcmp(args[3], "-z") != 0)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (strcmp(scenario, "attach-empty-worktrees") == 0) return answer(out, 0, "");
        if (strcmp(scenario, "attach-other-worktree") == 0)
            return answer(out, 0, "worktree other%cbranch refs/heads/main%c%c", 0, 0, 0);
        return answer(out, 0, "worktree %s%cdetached%c%c", path, 0, 0, 0);
    }
    if (strcmp(args[0], "update-ref") == 0) {
        if (count != 6U || strcmp(args[1], "-m") != 0 ||
            strcmp(args[3], "refs/heads/main") != 0 ||
            strcmp(args[4], repo->head) != 0 || strcmp(args[5], repo->local_main) != 0)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (strcmp(scenario, "attach-race") == 0) return answer(out, 1, "");
        (void)snprintf(repo->local_main, sizeof(repo->local_main), "%s", args[4]);
        ++workspace->advances;
        return answer(out, 0, "");
    }
    if (strcmp(args[0], "remote") == 0) {
        if (primary && count == 5U && (strcmp(scenario, "push-url") == 0 ||
            (strcmp(scenario, "late-push-url") == 0 && workspace->commits)))
            return answer(out, 0, "https://example.test/unexpected.git\n");
        return answer(out, 0, "https://example.test/%s.git\n", name);
    }
    if (strcmp(args[0], "status") == 0) {
        if (repo->dirty) return answer(out, 0, " M src/main.c%c", 0);
        return answer(out, 0, "");
    }
    if (strcmp(args[0], "fetch") == 0) {
        ++workspace->fetches;
        if (primary && strcmp(scenario, "refresh-failure-after-pull") == 0 && workspace->merges)
            return answer(out, 1, "");
        return answer(out, 0, "");
    }
    if (strcmp(args[0], "rev-list") == 0) {
        if (count == 4U) {
            if (primary && strcmp(scenario, "diverged") == 0) return answer(out, 0, "1\t1\n");
            if (strcmp(repo->head, remote) == 0) return answer(out, 0, "0\t0\n");
            if (repo->head[0] == '0' || (index == 2 && remote[0] == 'c'))
                return answer(out, 0, "0\t1\n");
            return answer(out, 0, "1\t0\n");
        }
        return answer(out, 0, "%s\n", repo->head);
    }
    if (strcmp(args[0], "ls-tree") == 0) {
        if (primary && (strcmp(scenario, "history-private") == 0 ||
            (strcmp(scenario, "hook-history") == 0 && workspace->commits)))
            return answer(out, 0, "Batch_chat.md%c", 0);
        return answer(out, 0, "src/main.c%c", 0);
    }
    if (strcmp(args[0], "ls-remote") == 0) {
        if (primary && strcmp(scenario, "remote-race") == 0)
            return answer(out, 0, "ffffffffffffffffffffffffffffffffffffffff\trefs/heads/main\n");
        return answer(out, 0, "%s\trefs/heads/main\n", remote);
    }
    /* Retain the old transport assertion for review. The active transport must
     * explicitly disable recursion, rebase and autostash even when Git config
     * enables them, and must pull exactly origin main. */
#if 0
    if (strcmp(args[0], "merge") == 0) {
        if (strcmp(args[1], "--ff-only") != 0 || count != 4U)
            return UMI_STATUS_INVALID_ARGUMENT;
        (void)snprintf(repo->head, sizeof(repo->head), "%s", args[3]);
        ++workspace->merges;
        return answer(out, 0, "");
    }
#endif
    if (strcmp(args[0], "-c") == 0) {
        const char *expected[] = {"-c", "submodule.recurse=false", "pull", "--ff-only",
            "--no-rebase", "--no-autostash", "--no-recurse-submodules", "origin", "main"};
        if (count != sizeof expected / sizeof expected[0]) return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t i = 0U; i < count; ++i)
            if (strcmp(args[i], expected[i]) != 0) return UMI_STATUS_INVALID_ARGUMENT;
        (void)snprintf(repo->head, sizeof(repo->head), "%s", remote);
        ++workspace->merges; /* Existing counter represents completed fast-forwards. */
        if (strcmp(scenario, "pull-destination-race") == 0) sha(repo->head, 'f');
        return answer(out, 0, "");
    }
    if (strcmp(args[0], "add") == 0) {
        if (count != 2U || strcmp(args[1], "-A") != 0) return UMI_STATUS_INVALID_ARGUMENT;
        ++workspace->adds;
        repo->staged = repo->dirty || (index == 1 && repo->pin[0] == 'c' && repo->head[0] == 'b');
        return answer(out, 0, "");
    }
    if (strcmp(args[0], "diff") == 0) {
        if (strcmp(args[count - 1U], "--check") == 0) {
            if (primary && strcmp(scenario, "check-failure") == 0) return answer(out, 2, "");
            return answer(out, 0, "");
        }
        return answer(out, repo->staged ? 1 : 0, "");
    }
    if (strcmp(args[0], "write-tree") == 0)
        return answer(out, 0, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee\n");
    if (strcmp(args[0], "commit") == 0) {
        if (primary && strcmp(scenario, "commit-failure") == 0) return answer(out, 1, "");
        if (count != 3U || strcmp(args[1], "-m") != 0 || !args[2][0])
            return UMI_STATUS_INVALID_ARGUMENT;
        ++workspace->commits;
        repo->dirty = repo->staged = 0;
        sha(repo->head, primary ? 'c' : 'd');
        return answer(out, 0, "");
    }
    if (strcmp(args[0], "push") == 0) {
        if (count != 5U || strcmp(args[1], "--recurse-submodules=check") != 0 ||
            strcmp(args[2], "--no-follow-tags") != 0 ||
            strcmp(args[4], "refs/heads/main:refs/heads/main") != 0)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (primary && strcmp(scenario, "push-failure") == 0) return answer(out, 1, "");
        ++workspace->pushes;
        if (workspace->sequence_count >= 32U) return UMI_STATUS_CAPACITY_EXCEEDED;
        workspace->sequence[workspace->sequence_count++] = primary ? 1 : 3;
        (void)snprintf(remote, 65U, "%s", repo->head);
        return answer(out, 0, "");
    }
    (void)fprintf(stderr, "Unexpected fake Git operation: %s\n", args[0]);
    return UMI_STATUS_NOT_IMPLEMENTED;
}

static UmiStatus fake_lock(void *context, const char *path, int dry_run)
{
    FakeWorkspace *workspace = context;
    if (repository_index(workspace, path) != 1) return UMI_STATUS_INVALID_ARGUMENT;
    if (dry_run) return UMI_STATUS_OK;
    /* A parent's actual lock must occur after its child's successful push and
     * after that commit reaches the child copy used by this parent. */
    if (workspace->child_remote[0] != workspace->repos[2].head[0])
        return UMI_STATUS_INVALID_STATE;
    ++workspace->locks;
    if (workspace->sequence_count >= 32U) return UMI_STATUS_CAPACITY_EXCEEDED;
    workspace->sequence[workspace->sequence_count++] = 2;
    (void)snprintf(workspace->repos[1].pin, 65U, "%s", workspace->repos[2].head);
    return UMI_STATUS_OK;
}

static UmiStatus create_fixture(FakeWorkspace *workspace, const char *scenario, unsigned number)
{
    char temp[UMI_PATH_CAPACITY], leaf[128], metadata[UMI_PATH_CAPACITY];
    UmiClock clock = umi_clock_system();
    memset(workspace, 0, sizeof(*workspace));
    workspace->scenario = scenario;
    UmiStatus status = umi_fs_temp_directory(temp, sizeof(temp));
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(leaf, sizeof(leaf), "umicom-publication-%llu-%u",
        (unsigned long long)clock.monotonic_nanoseconds(&clock), number);
    status = umi_path_join(temp, leaf, workspace->root, sizeof(workspace->root));
    if (status != UMI_STATUS_OK || umi_fs_exists(workspace->root)) return UMI_STATUS_ALREADY_EXISTS;
    status = umi_fs_make_directories(workspace->root);
    if (status != UMI_STATUS_OK) return status;
    workspace->owns_fixture = 1;
    const char *names[] = {"a", "b", "b/module"};
    for (size_t i = 0U; i < 3U; ++i) {
        status = umi_path_join(workspace->root, names[i], workspace->repos[i].path,
            sizeof(workspace->repos[i].path));
        if (status != UMI_STATUS_OK) return status;
        status = umi_path_join(workspace->repos[i].path, ".git", metadata, sizeof(metadata));
        if (status != UMI_STATUS_OK) return status;
        status = umi_fs_make_directories(metadata);
        if (status != UMI_STATUS_OK) return status;
        sha(workspace->repos[i].head, i == 1U ? 'b' : 'a');
        sha(workspace->repos[i].local_main, i == 1U ? 'b' : 'a');
    }
    sha(workspace->repos[1].pin, 'a');
    sha(workspace->child_remote, 'a');
    sha(workspace->parent_remote, 'b');
    workspace->repos[0].dirty = 1;
    if (strcmp(scenario, "duplicate-dirty") == 0) workspace->repos[2].dirty = 1;
    if (strcmp(scenario, "behind-copy") == 0) sha(workspace->repos[2].head, '0');
    /* A clean-only update cannot regain its lost progress flag through a second
     * publication update. It exposes evidence lost during graph rediscovery. */
    if (strcmp(scenario, "clean-behind-only") == 0 ||
        strcmp(scenario, "refresh-failure-after-pull") == 0 ||
        strcmp(scenario, "pull-destination-race") == 0 ||
        strcmp(scenario, "pull-head-read-failure") == 0) {
        workspace->repos[0].dirty = 0;
        sha(workspace->repos[2].head, '0');
    }
    if (strcmp(scenario, "dirty-behind") == 0) sha(workspace->repos[0].head, '0');
    if (strcmp(scenario, "unpublished") == 0 || strcmp(scenario, "history-private") == 0) {
        workspace->repos[0].dirty = 0;
        sha(workspace->repos[0].head, 'c');
    }
    if (strcmp(scenario, "existing-lock") == 0) {
        status = umi_path_join(workspace->repos[0].path, ".git/umicom-publication.lock",
            metadata, sizeof(metadata));
        if (status != UMI_STATUS_OK) return status;
        return umi_fs_write_text(metadata, "Another publisher owns this marker.\n");
    }
    if (strcmp(scenario, "missing-child") == 0) {
        status = umi_path_join(workspace->repos[2].path, ".git", metadata, sizeof(metadata));
        if (status != UMI_STATUS_OK) return status;
        return umi_fs_remove_tree(metadata);
    }
    return UMI_STATUS_OK;
}

static int run_case(const char *scenario, unsigned number)
{
    FakeWorkspace *workspace = calloc(1U, sizeof(*workspace));
    UmiRepositoryPublicationReport *report = calloc(1U, sizeof(*report));
    UmiRepositoryPublicationRequest request;
    int passed = 0;
    if (!workspace || !report) goto finish;
    if (create_fixture(workspace, scenario, number) != UMI_STATUS_OK) goto finish;
    UmiPublicationBackend backend = {workspace, fake_git, fake_lock};
    umi_repository_publication_request_init(&request, workspace->root);
    request.apply = strcmp(scenario, "preview") != 0 && strcmp(scenario, "watch") != 0;
    request.refresh = strcmp(scenario, "watch") != 0;
    UmiStatus status = umi_publication_run_backend(NULL, NULL, &request, report, &backend);
    if (strcmp(scenario, "success") == 0 || strcmp(scenario, "behind-copy") == 0) {
        passed = status == UMI_STATUS_OK && report->count == 3U &&
            report->repository_count == 2U && workspace->commits == 2U &&
            workspace->pushes == 2U && workspace->sequence_count == 3U &&
            workspace->sequence[0] == 1 && workspace->sequence[1] == 2 &&
            workspace->sequence[2] == 3 && workspace->repos[2].head[0] == 'c';
        /* Repeating a successful apply creates no duplicate commits or pushes. */
        size_t commits = workspace->commits, pushes = workspace->pushes;
        status = umi_publication_run_backend(NULL, NULL, &request, report, &backend);
        passed = passed && status == UMI_STATUS_OK &&
            workspace->commits == commits && workspace->pushes == pushes;
    } else if (strcmp(scenario, "unpublished") == 0) {
        passed = status == UMI_STATUS_OK && workspace->commits == 1U && workspace->pushes == 2U;
    } else if (strcmp(scenario, "preview") == 0 || strcmp(scenario, "watch") == 0) {
        passed = status == UMI_STATUS_OK && workspace->adds == 0U &&
            workspace->commits == 0U && workspace->pushes == 0U &&
            workspace->locks == 0U && workspace->merges == 0U;
        if (strcmp(scenario, "watch") == 0) passed = passed && workspace->fetches == 0U;
    } else if (strcmp(scenario, "push-failure") == 0 || strcmp(scenario, "hook-tree") == 0 ||
        strcmp(scenario, "late-push-url") == 0 || strcmp(scenario, "hook-history") == 0) {
        passed = status != UMI_STATUS_OK && workspace->commits == 1U &&
            workspace->pushes == 0U && workspace->locks == 0U &&
            workspace->repos[0].head[0] == 'c';
    } else {
        passed = status != UMI_STATUS_OK && workspace->commits == 0U &&
            workspace->pushes == 0U && workspace->locks == 0U;
    }
    /* Inspect retained evidence independently of the fake remote's state. */
    if (strcmp(scenario, "clean-behind-only") == 0 ||
        strcmp(scenario, "refresh-failure-after-pull") == 0 ||
        strcmp(scenario, "pull-destination-race") == 0 ||
        strcmp(scenario, "pull-head-read-failure") == 0) {
        UmiRepositoryPublicationEntry *updated = NULL;
        for (size_t i = 0U; i < report->count; ++i)
            if (umi_path_equal(report->entries[i].path, workspace->repos[2].path)) updated = &report->entries[i];
        passed = updated && updated->fast_forwarded && report->updated_count == 1U &&
            workspace->commits == 0U && workspace->pushes == 0U &&
            (strcmp(scenario, "clean-behind-only") == 0 ? status == UMI_STATUS_OK : status != UMI_STATUS_OK);
        if (strcmp(scenario, "pull-destination-race") == 0)
            passed = passed && updated->head[0] == 'f';
    }
    if (strcmp(scenario, "hook-tree") == 0 || strcmp(scenario, "push-failure") == 0) {
        UmiRepositoryPublicationEntry *committed = NULL;
        for (size_t i = 0U; i < report->count; ++i)
            if (umi_path_equal(report->entries[i].path, workspace->repos[0].path)) committed = &report->entries[i];
        passed = passed && committed && committed->committed && !committed->pushed && committed->head[0] == 'c';
    }
    if (strcmp(scenario, "existing-lock") == 0) {
        char marker[UMI_PATH_CAPACITY];
        passed = passed && umi_path_join(workspace->repos[0].path,
            ".git/umicom-publication.lock", marker, sizeof(marker)) == UMI_STATUS_OK &&
            umi_fs_exists(marker);
    }
    if (!passed) (void)fprintf(stderr, "Scenario %s: status=%d, commits=%zu, pushes=%zu, locks=%zu; %s\n",
        scenario, (int)status, workspace->commits, workspace->pushes, workspace->locks, report->detail);
finish:
    /* This unique fixture is test-owned. Never delete a caller's repository. */
    if (workspace && workspace->owns_fixture && umi_fs_exists(workspace->root))
        if (umi_fs_remove_tree(workspace->root) != UMI_STATUS_OK) passed = 0;
    free(workspace);
    free(report);
    return passed;
}


/* These cases execute the complete planner through a fake transport. They
 * prove that attachment never commits files, pushes, or guesses conflict text. */
static int run_attachment_case(const char *scenario, unsigned number)
{
    FakeWorkspace *workspace = calloc(1U, sizeof(*workspace));
    UmiRepositoryPublicationReport *report = calloc(1U, sizeof(*report));
    UmiRepositoryPublicationRequest request;
    int passed = 0;
    if (!workspace || !report) goto finish;
    if (create_fixture(workspace, scenario, number) != UMI_STATUS_OK) goto finish;
    FakeRepository *repo = &workspace->repos[0];
    repo->dirty = strcmp(scenario, "attach-dirty") == 0;
    repo->detached = 1;
    sha(repo->local_main, strcmp(scenario, "attach-at-tip") == 0 ? 'a' : '0');
    if (strcmp(scenario, "attach-unpublished") == 0) sha(repo->head, 'c');
    UmiPublicationBackend backend = {workspace, fake_git, fake_lock};
    umi_repository_publication_request_init(&request, workspace->root);
    request.apply = strcmp(scenario, "attach-preview") != 0;
    request.refresh = strcmp(scenario, "attach-without-refresh") != 0;
    UmiStatus status = umi_publication_run_recovery_backend(NULL, NULL, &request,
        report, &backend, 1);
    if (strcmp(scenario, "attach-preview") == 0) {
        passed = status == UMI_STATUS_OK && repo->detached &&
            workspace->advances == 0U && workspace->attachments == 0U &&
            workspace->adds == 0U && workspace->locks == 0U;
    } else if (strcmp(scenario, "attach-success") == 0 || strcmp(scenario, "attach-at-tip") == 0) {
        passed = status == UMI_STATUS_OK && !repo->detached &&
            strcmp(repo->head, repo->local_main) == 0 && workspace->attachments == 1U &&
            workspace->advances == (strcmp(scenario, "attach-at-tip") == 0 ? 0U : 1U) &&
            report->updated_count == 1U;
        /* Repeating recovery is a no-op, including the already-published branch. */
        size_t advances = workspace->advances;
        status = umi_publication_run_recovery_backend(NULL, NULL, &request, report, &backend, 1);
        passed = passed && status == UMI_STATUS_OK && workspace->attachments == 1U &&
            workspace->advances == advances && report->updated_count == 0U;
    } else {
        passed = status != UMI_STATUS_OK && repo->detached && workspace->attachments == 0U &&
            workspace->advances == (strcmp(scenario, "attach-bind-failure") == 0 ? 1U : 0U) &&
            workspace->adds == 0U && workspace->locks == 0U;
    }
    passed = passed && workspace->commits == 0U && workspace->pushes == 0U &&
        workspace->merges == 0U && repo->dirty == (strcmp(scenario, "attach-dirty") == 0) &&
        repo->head[0] == (strcmp(scenario, "attach-unpublished") == 0 ? 'c' : 'a');
    if (!passed) (void)fprintf(stderr,
        "Recovery %s: status=%d advances=%zu attachments=%zu; %s\n", scenario,
        (int)status, workspace->advances, workspace->attachments, report->detail);
finish:
    if (workspace && workspace->owns_fixture && umi_fs_exists(workspace->root))
        if (umi_fs_remove_tree(workspace->root) != UMI_STATUS_OK) passed = 0;
    free(workspace);
    free(report);
    return passed;
}

int main(void)
{
    const char *scenarios[] = {"success", "preview", "watch", "duplicate-dirty",
        "behind-copy", "detached", "diverged", "private", "late-private",
        "history-private", "unpublished", "push-url", "remote-race",
        "check-failure", "commit-failure", "push-failure", "hook-tree",
        "existing-lock", "missing-child", "stage-conflict", "malformed-index",
        "index-change", "dirty-behind", "late-push-url", "hook-history",
        "clean-behind-only", "refresh-failure-after-pull", "pull-destination-race", "pull-head-read-failure"};
    int passed = 1;
    for (unsigned i = 0U; i < sizeof(scenarios) / sizeof(scenarios[0]); ++i)
        passed = run_case(scenarios[i], i) && passed;
    const char *recovery[] = {"attach-preview", "attach-success", "attach-at-tip",
        "attach-dirty", "attach-unpublished", "attach-divergent-main",
        "attach-other-worktree", "attach-race", "attach-bind-failure",
        "attach-missing-main", "attach-empty-worktrees", "attach-without-refresh"};
    for (unsigned i = 0U; i < sizeof(recovery) / sizeof(recovery[0]); ++i)
        passed = run_attachment_case(recovery[i], 100U + i) && passed;
    return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
