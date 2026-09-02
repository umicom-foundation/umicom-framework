/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_fixture.h
 *
 * PURPOSE:
 *   Verify the vcs fixture behaviour and report a clear failure when its
 *   contract changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Deterministic Framework VCS provider fixture. Sammy Hegab, Umicom Foundation. MIT. */
#ifndef UMICOM_TESTS_VCS_FIXTURE_H
#define UMICOM_TESTS_VCS_FIXTURE_H
#include <stdio.h>
#include <string.h>
#include "umicom/umicom.h"
/**
 * Represent the test vcs fixture data shared with callers of this public contract.
 */
typedef struct UmiTestVcsFixture {
    size_t stage_calls, unstage_calls, stage_all_calls, unstage_all_calls;
    size_t discard_calls, commit_calls, fetch_calls, pull_calls, push_calls;
    size_t create_calls, checkout_calls, delete_calls, diff_calls;
} UmiTestVcsFixture;
/**
 * Exercise test vcs status and return a clear result when the behaviour no longer matches
 * its contract.
 */
static inline UmiStatus test_vcs_status(void *instance, const char *root, UmiVcsChangeList *changes, UmiVcsBranch *branch)
{
    UmiVcsChange change = {0}; (void)instance; (void)root; umi_vcs_change_list_clear(changes);
    (void)snprintf(change.path, sizeof(change.path), "%s", "src/main.c");
    change.index_state = UMI_VCS_CHANGE_MODIFIED; change.worktree_state = UMI_VCS_CHANGE_UNMODIFIED; change.staged = 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_change_list_add(changes, &change) != UMI_STATUS_OK) return UMI_STATUS_INTERNAL_ERROR;
    (void)memset(branch, 0, sizeof(*branch)); (void)snprintf(branch->name, sizeof(branch->name), "%s", "main");
    (void)snprintf(branch->upstream, sizeof(branch->upstream), "%s", "origin/main"); branch->current = 1; branch->ahead = 1;
    return UMI_STATUS_OK;
}
/**
 * Exercise test vcs history and return a clear result when the behaviour no longer matches
 * its contract.
 */
static inline UmiStatus test_vcs_history(void *instance, const char *root, size_t limit, UmiVcsHistory *history)
{ (void)instance; (void)root; (void)limit; return umi_vcs_history_parse(history, "abc123" "\x1f" "1700000000" "\x1f" "Sammy Hegab" "\x1f" "sammy@example.com" "\x1f" "Framework source control\n", NULL); }
/**
 * Exercise test vcs stage and return a clear result when the behaviour no longer matches
 * its contract.
 */
static inline UmiStatus test_vcs_stage(void *instance, const char *root, const char *path)
{ (void)root; (void)path; ((UmiTestVcsFixture *)instance)->stage_calls++; return UMI_STATUS_OK; }
/**
 * Exercise test vcs unstage and return a clear result when the behaviour no longer matches
 * its contract.
 */
static inline UmiStatus test_vcs_unstage(void *instance, const char *root, const char *path)
{ (void)root; (void)path; ((UmiTestVcsFixture *)instance)->unstage_calls++; return UMI_STATUS_OK; }
/**
 * Exercise test vcs commit and return a clear result when the behaviour no longer matches
 * its contract.
 */
static inline UmiStatus test_vcs_commit(void *instance, const char *root, const char *message, char *out_id, size_t capacity)
{ (void)root; (void)message; ((UmiTestVcsFixture *)instance)->commit_calls++; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (out_id != NULL && capacity >= 8U) (void)snprintf(out_id, capacity, "%s", "abc1234"); return UMI_STATUS_OK; }
/**
 * Exercise test vcs pull and return a clear result when the behaviour no longer matches
 * its contract.
 */
static inline UmiStatus test_vcs_pull(void *instance, const char *root)
{ (void)root; ((UmiTestVcsFixture *)instance)->pull_calls++; return UMI_STATUS_OK; }
/**
 * Exercise test vcs push and return a clear result when the behaviour no longer matches
 * its contract.
 */
static inline UmiStatus test_vcs_push(void *instance, const char *root)
{ (void)root; ((UmiTestVcsFixture *)instance)->push_calls++; return UMI_STATUS_OK; }
/**
 * Exercise test vcs branches and return a clear result when the behaviour no longer
 * matches its contract.
 */
static inline UmiStatus test_vcs_branches(void *instance, const char *root, UmiVcsBranchList *list)
{ UmiVcsBranch branch = {0}; (void)instance; (void)root; umi_vcs_branch_list_clear(list); (void)snprintf(branch.name, sizeof(branch.name), "%s", "main"); branch.current = 1; return umi_vcs_branch_list_add(list, &branch); }
/**
 * Exercise test vcs remotes and return a clear result when the behaviour no longer matches
 * its contract.
 */
static inline UmiStatus test_vcs_remotes(void *instance, const char *root, UmiVcsRemoteList *list)
{ UmiVcsRemote remote = {0}; (void)instance; (void)root; umi_vcs_remote_list_clear(list); (void)snprintf(remote.name, sizeof(remote.name), "%s", "origin"); (void)snprintf(remote.fetch_url, sizeof(remote.fetch_url), "%s", "https://example.com/umicom.git"); return umi_vcs_remote_list_upsert(list, &remote); }
/**
 * Exercise test vcs tags and return a clear result when the behaviour no longer matches
 * its contract.
 */
static inline UmiStatus test_vcs_tags(void *instance, const char *root, UmiVcsTagList *list)
{ UmiVcsTag tag = {0}; (void)instance; (void)root; umi_vcs_tag_list_clear(list); (void)snprintf(tag.name, sizeof(tag.name), "%s", "v0.8.0"); (void)snprintf(tag.target_id, sizeof(tag.target_id), "%s", "abc123"); return umi_vcs_tag_list_add(list, &tag); }
/**
 * Exercise test vcs diff and return a clear result when the behaviour no longer matches
 * its contract.
 */
static inline UmiStatus test_vcs_diff(void *instance, const char *root, const char *path, int staged, char *out_text, size_t capacity)
{ (void)root; (void)path; (void)staged; ((UmiTestVcsFixture *)instance)->diff_calls++; (void)snprintf(out_text, capacity, "%s", "diff --git a/src/main.c b/src/main.c\n@@ -1 +1 @@\n-old\n+new\n"); return UMI_STATUS_OK; }
/**
 * Exercise test vcs stage all and return a clear result when the behaviour no longer
 * matches its contract.
 */
static inline UmiStatus test_vcs_stage_all(void *instance, const char *root)
{ (void)root; ((UmiTestVcsFixture *)instance)->stage_all_calls++; return UMI_STATUS_OK; }
/**
 * Exercise test vcs unstage all and return a clear result when the behaviour no longer
 * matches its contract.
 */
static inline UmiStatus test_vcs_unstage_all(void *instance, const char *root)
{ (void)root; ((UmiTestVcsFixture *)instance)->unstage_all_calls++; return UMI_STATUS_OK; }
/**
 * Exercise test vcs discard and return a clear result when the behaviour no longer matches
 * its contract.
 */
static inline UmiStatus test_vcs_discard(void *instance, const char *root, const char *path)
{ (void)root; (void)path; ((UmiTestVcsFixture *)instance)->discard_calls++; return UMI_STATUS_OK; }
/**
 * Exercise test vcs fetch and return a clear result when the behaviour no longer matches
 * its contract.
 */
static inline UmiStatus test_vcs_fetch(void *instance, const char *root)
{ (void)root; ((UmiTestVcsFixture *)instance)->fetch_calls++; return UMI_STATUS_OK; }
/**
 * Initialise test vcs branch from caller-provided values so later operations receive a
 * known state.
 */
static inline UmiStatus test_vcs_branch_create(void *instance, const char *root, const char *name, int checkout)
{ (void)root; (void)name; (void)checkout; ((UmiTestVcsFixture *)instance)->create_calls++; return UMI_STATUS_OK; }
/**
 * Exercise test vcs branch checkout and return a clear result when the behaviour no longer
 * matches its contract.
 */
static inline UmiStatus test_vcs_branch_checkout(void *instance, const char *root, const char *name)
{ (void)root; (void)name; ((UmiTestVcsFixture *)instance)->checkout_calls++; return UMI_STATUS_OK; }
/**
 * Exercise test vcs branch delete and return a clear result when the behaviour no longer
 * matches its contract.
 */
static inline UmiStatus test_vcs_branch_delete(void *instance, const char *root, const char *name, int force)
{ (void)root; (void)name; (void)force; ((UmiTestVcsFixture *)instance)->delete_calls++; return UMI_STATUS_OK; }
/**
 * Exercise test vcs provider and return a clear result when the behaviour no longer
 * matches its contract.
 */
static inline UmiVcsProvider test_vcs_provider(UmiTestVcsFixture *fixture)
{
    UmiVcsProvider provider; (void)memset(&provider, 0, sizeof(provider)); provider.structure_size = (uint32_t)sizeof(provider);
    provider.provider_id = "umicom.test-vcs"; provider.instance = fixture; provider.status = test_vcs_status; provider.history = test_vcs_history;
    provider.stage = test_vcs_stage; provider.unstage = test_vcs_unstage; provider.commit = test_vcs_commit; provider.pull = test_vcs_pull; provider.push = test_vcs_push;
    provider.branches = test_vcs_branches; provider.remotes = test_vcs_remotes; provider.tags = test_vcs_tags; provider.diff = test_vcs_diff;
    provider.stage_all = test_vcs_stage_all; provider.unstage_all = test_vcs_unstage_all; provider.discard = test_vcs_discard; provider.fetch = test_vcs_fetch;
    provider.branch_create = test_vcs_branch_create; provider.branch_checkout = test_vcs_branch_checkout; provider.branch_delete = test_vcs_branch_delete;
    return provider;
}
#endif
