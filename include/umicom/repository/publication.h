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

/** Classify a repository-relative path before broad staging. Returns nonzero
 * for private working records or generated build outputs requiring review. */
int umi_repository_publication_private_path(const char *path);

#ifdef __cplusplus
}
#endif
#endif
