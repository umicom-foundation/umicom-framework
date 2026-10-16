/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/publication_internal.h
 *
 * PURPOSE:
 *   Share bounded publication state and an injectable Git transport for regression tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PUBLICATION_INTERNAL_H
#define UMICOM_PUBLICATION_INTERNAL_H
#include "umicom/repository/publication.h"
#include "umicom/platform/directory.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/process.h"

#define UMI_PUBLICATION_EDGE_CAPACITY 512U
#define UMI_PUBLICATION_CAPTURE_LIMIT (16U * 1024U * 1024U)
#define UMI_PUBLICATION_HISTORY_LIMIT 256U

typedef struct UmiPublicationCapture {
    char *bytes;
    size_t size;
    int exit_code;
} UmiPublicationCapture;

/* The test seam replaces process execution, not planning or sequencing.
 * Transport owns the returned malloc buffer; the service frees it. */
typedef struct UmiPublicationBackend {
    void *context;
    UmiStatus (*git)(void *, const char *, const char *const *, size_t,
        UmiPublicationCapture *);
    UmiStatus (*lock)(void *, const char *, int);
} UmiPublicationBackend;

typedef struct UmiPublicationEdge {
    size_t parent;
    size_t child;
    char path[UMI_PATH_CAPACITY];
    char pinned[65];
} UmiPublicationEdge;

typedef struct UmiPublicationState {
    const UmiToolchainProfile *profile;
    UmiEnvironmentPlan *environment;
    const UmiRepositoryPublicationRequest *request;
    UmiRepositoryPublicationReport *report;
    UmiPublicationBackend backend;
    /* Recovery is a separate opt-in; ordinary publication never attaches HEAD. */
    int attach_published_main;
    unsigned char attach_ready[UMI_REPOSITORY_PUBLICATION_CAPACITY];
    char previous_main[UMI_REPOSITORY_PUBLICATION_CAPACITY][65];
    char root[UMI_PATH_CAPACITY];
    char identity[UMI_REPOSITORY_PUBLICATION_CAPACITY][1024];
    UmiPublicationEdge edges[UMI_PUBLICATION_EDGE_CAPACITY];
    size_t edge_count;
    unsigned char dirty[UMI_REPOSITORY_PUBLICATION_CAPACITY];
    size_t order[UMI_REPOSITORY_PUBLICATION_CAPACITY];
    size_t order_count;
    size_t lock_count;
    char lock_paths[UMI_REPOSITORY_PUBLICATION_CAPACITY][UMI_PATH_CAPACITY];
    unsigned char visiting[UMI_REPOSITORY_PUBLICATION_CAPACITY];
} UmiPublicationState;

UmiStatus umi_publication_native_git(void *, const char *,
    const char *const *, size_t, UmiPublicationCapture *);
UmiStatus umi_publication_native_lock(void *, const char *, int);
UmiStatus umi_publication_git(UmiPublicationState *, size_t,
    const char *const *, size_t, int, UmiPublicationCapture *);
UmiStatus umi_publication_text(UmiPublicationState *, size_t,
    const char *const *, size_t, char *, size_t);
UmiStatus umi_publication_fail(UmiPublicationState *, size_t,
    UmiStatus, const char *);
UmiStatus umi_publication_discover(UmiPublicationState *);
UmiStatus umi_publication_inspect(UmiPublicationState *, size_t);
UmiStatus umi_publication_plan(UmiPublicationState *);
UmiStatus umi_publication_execute(UmiPublicationState *);
UmiStatus umi_publication_acquire_locks(UmiPublicationState *);
UmiStatus umi_publication_release_locks(UmiPublicationState *);
UmiStatus umi_publication_verify_links(UmiPublicationState *, size_t);
UmiStatus umi_publication_update_initial(UmiPublicationState *, int *);
UmiStatus umi_publication_check_paths(UmiPublicationState *, size_t,
    const char *const *, size_t);
UmiStatus umi_publication_check_history(UmiPublicationState *, size_t);
UmiStatus umi_publication_origin_identity(const char *, char *, size_t);
UmiStatus umi_publication_safe_path(UmiPublicationState *, const char *);
UmiStatus umi_publication_run_backend(const UmiToolchainProfile *,
    UmiEnvironmentPlan *, const UmiRepositoryPublicationRequest *,
    UmiRepositoryPublicationReport *, const UmiPublicationBackend *);
UmiStatus umi_publication_branch(UmiPublicationState *, size_t);
UmiStatus umi_publication_attachment_check(UmiPublicationState *, size_t);
UmiStatus umi_publication_attach_published(UmiPublicationState *, size_t);
UmiStatus umi_publication_run_recovery_backend(const UmiToolchainProfile *,
    UmiEnvironmentPlan *, const UmiRepositoryPublicationRequest *,
    UmiRepositoryPublicationReport *, const UmiPublicationBackend *, int);
#endif
