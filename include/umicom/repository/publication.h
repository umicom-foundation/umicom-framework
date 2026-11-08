/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/publication.h
 *
 * PURPOSE:
 *   Plan and publish related Git checkouts without shell scripts or history rewriting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_REPOSITORY_PUBLICATION_H
#define UMICOM_REPOSITORY_PUBLICATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/platform/cancellation.h"
#include "umicom/platform/path.h"
#include "umicom/toolchain/environment.h"
#include "umicom/toolchain/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_REPOSITORY_PUBLICATION_CAPACITY 128U

/** One invocation discovers either a repository and its registered children,
 * or the immediate repositories in a workspace and their registered children.
 * apply=0 never stages, commits, pushes or changes working files. refresh=1
 * fetches origin/main metadata even in preview; refresh=0 uses cached refs.
 * Applying requires refresh=1. All strings and cancellation remain borrowed
 * until return. Call on a worker thread and keep editors idle during apply. */
typedef struct UmiRepositoryPublicationRequest {
    const char *workspace_root;
    const char *commit_message;
    int apply;
    int refresh;
    uint32_t timeout_ms;
    const UmiCancellationToken *cancellation;
    /* Optional synchronous progress callback; strings are borrowed for the call. */
    void (*observer)(const char *path, const char *action, void *context);
    void *observer_context;
} UmiRepositoryPublicationRequest;

/** A checkout remains individually visible even when another copy publishes
 * the same remote. primary identifies that selected row; parent is SIZE_MAX
 * for a top-level checkout. Vendor children are observed, never published. */
typedef struct UmiRepositoryPublicationEntry {
    char path[UMI_PATH_CAPACITY];
    char head[65];
    char remote_head[65];
    char branch[128];
    char detail[512];
    size_t parent;
    size_t primary;
    size_t changed_paths;
    size_t ahead;
    size_t behind;
    int vendor;
    int blocked;
    int committed;
    int pushed;
    int fast_forwarded;
    int locked;
} UmiRepositoryPublicationEntry;

/** Allocate this bounded report on the heap. On any failure it retains the
 * checkout and completed actions so an interrupted run can be investigated.
 * Publication is not a transaction across remotes; nothing is rolled back. */
typedef struct UmiRepositoryPublicationReport {
    UmiStatus status;
    size_t count;
    size_t repository_count;
    size_t blocked_count;
    size_t committed_count;
    size_t pushed_count;
    size_t updated_count;
    char detail[512];
    UmiRepositoryPublicationEntry entries[UMI_REPOSITORY_PUBLICATION_CAPACITY];
} UmiRepositoryPublicationReport;

/** Fill defaults: preview, refresh enabled, and a five-minute Git timeout. */
void umi_repository_publication_request_init(
    UmiRepositoryPublicationRequest *request, const char *workspace_root);

/** Discover, preflight and optionally publish in child-before-parent order.
 * Dirty duplicate copies, non-main owned repositories, divergent histories,
 * missing children and known private paths block publication. Clean copies
 * can fast-forward; no checkout, reset, stash, force, rebase or merge commit
 * is used. Existing single-repository workflows remain unchanged.
 *
 * Path protection is a conservative filename policy, not a content/secret
 * scanner. The caller must review what is suitable for publication. Existing
 * Git hooks and credential helpers run normally. Use trusted repositories. */
UmiStatus umi_repository_publication_run_sized(
    const UmiToolchainProfile *profile, UmiEnvironmentPlan *environment,
    const UmiRepositoryPublicationRequest *request,
    UmiRepositoryPublicationReport *report, size_t report_size);

#define umi_repository_publication_run(profile, environment, request, report) \
    umi_repository_publication_run_sized((profile), (environment), (request), \
        (report), sizeof(*(report)))

/** Preview or apply narrowly bounded recovery alongside publication.
 * attach_published_main must be 0 or 1. When enabled, only a clean detached
 * leaf checkout exactly at fetched origin/main may attach to an existing main
 * branch whose history is an ancestor of that commit. Main must not be checked
 * out elsewhere. No source files or index entries are rewritten. Unique local
 * commits, dirty copies and true conflicts still require owner review.
 * Preview never changes refs other than the normal fetch. Existing publication
 * entry points retain their original no-attachment behavior. */
UmiStatus umi_repository_publication_run_recovery_sized(
    const UmiToolchainProfile *profile, UmiEnvironmentPlan *environment,
    const UmiRepositoryPublicationRequest *request,
    UmiRepositoryPublicationReport *report, size_t report_size,
    int attach_published_main);

#define umi_repository_publication_run_recovery(profile, environment, request, report, attach) \
    umi_repository_publication_run_recovery_sized((profile), (environment), (request), \
        (report), sizeof(*(report)), (attach))

/** Immutable staged snapshot offered before creating a new commit. Buffers and
 * strings are borrowed only during the callback. They can contain source code;
 * keep them local and render terminal controls as data. The diff compares the
 * captured HEAD with the captured tree, not a changing worktree or index. */
typedef struct UmiRepositoryPublicationStagedReview {
    const char *repository_path;
    const char *head;
    const char *tree;
    const char *summary;
    size_t summary_size;
    const char *patch;
    size_t patch_size;
} UmiRepositoryPublicationStagedReview;

typedef UmiStatus (*UmiRepositoryPublicationReviewCallback)(
    const UmiRepositoryPublicationStagedReview *review, void *context);

/** Optional policy for callers that maintain one primary editing directory.
 * primary_directory is borrowed, may be absolute or relative to workspace_root,
 * and must contain at least one discovered non-vendor checkout. For each remote
 * represented there, exactly one checkout may be inside that directory. Other
 * copies must have no independent work. Repositories not represented there keep
 * the existing selection policy. NULL preserves the original behavior.
 *
 * This chooses a publishing checkout; it does not copy or reconcile source.
 * attach_published_main has the same explicit recovery contract as above. */
typedef struct UmiRepositoryPublicationOptions {
    const char *primary_directory;
    int attach_published_main;
    /* Optional synchronous review before each NEW commit. Return OK to proceed;
     * any other status stops publication while preserving the staged index and
     * all earlier completed actions. Preview/watch never invoke this callback.
     * It does not review pre-existing unpublished commits. Keep editors idle;
     * index and HEAD are rechecked after review, without a filesystem transaction. */
    UmiRepositoryPublicationReviewCallback review_staged;
    void *review_context;
} UmiRepositoryPublicationOptions;

void umi_repository_publication_options_init(UmiRepositoryPublicationOptions *options);

/** Apply optional policy without changing the layout of an existing request.
 * Exact structure sizes are checked before reading options or writing reports.
 * Options and all borrowed strings remain valid until the synchronous return. */
UmiStatus umi_repository_publication_run_options_sized(
    const UmiToolchainProfile *profile, UmiEnvironmentPlan *environment,
    const UmiRepositoryPublicationRequest *request,
    const UmiRepositoryPublicationOptions *options, size_t options_size,
    UmiRepositoryPublicationReport *report, size_t report_size);

#define umi_repository_publication_run_options(profile, environment, request, options, report) \
    umi_repository_publication_run_options_sized((profile), (environment), (request), \
        (options), sizeof(*(options)), (report), sizeof(*(report)))


#define UMI_REPOSITORY_PUBLICATION_EXCLUSION_CAPACITY 16U
/* Exclusions name immediate checkout directories, not paths or remote IDs.
 * Names use 1..63 ASCII letters/digits/dot/underscore/hyphen, start with an
 * alphanumeric byte, and total at most 256 bytes including separators.
 * A scoped workspace must not itself be a Git checkout. Every excluded name
 * must match an immediate checkout; registered children cannot be excluded.
 * Existing APIs and structure layouts keep their original discovery scope. */
UmiStatus umi_repository_publication_exclusions_validate(
    const char *const *names, size_t count);
UmiStatus umi_repository_publication_run_scoped_sized(
    const UmiToolchainProfile *profile, UmiEnvironmentPlan *environment,
    const UmiRepositoryPublicationRequest *request,
    const UmiRepositoryPublicationOptions *options, size_t options_size,
    const char *const *excluded_names, size_t excluded_count,
    UmiRepositoryPublicationReport *report, size_t report_size);
#define umi_repository_publication_run_scoped(profile, environment, request, options, names, count, report) \
    umi_repository_publication_run_scoped_sized((profile), (environment), (request), \
        (options), sizeof(*(options)), (names), (count), (report), sizeof(*(report)))

/** Classify a repository-relative path before broad staging. Returns nonzero
 * for private working records or generated build outputs requiring review. */
int umi_repository_publication_private_path(const char *path);

#ifdef __cplusplus
}
#endif
#endif
