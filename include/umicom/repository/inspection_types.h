/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/inspection_types.h
 *
 * PURPOSE:
 *   Define bounded inspection state shared by read-only Git-backed repository probes.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_REPOSITORY_INSPECTION_TYPES_H
#define UMICOM_REPOSITORY_INSPECTION_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/repository/status_summary.h"
#include "umicom/repository/maintenance_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_REPOSITORY_INSPECTION_TEXT_CAPACITY 2048U
#define UMI_REPOSITORY_INSPECTION_OUTPUT_CAPACITY 65536U
#define UMI_REPOSITORY_INSPECTION_ISSUE_CAPACITY 64U
#define UMI_REPOSITORY_INSPECTION_TAG_CAPACITY 64U

/*
 * Inspection extensions reuse the canonical repository-doctor severity type so
 * CLI and workbench consumers see one health vocabulary across all findings.
 */
typedef UmiRepositoryDoctorSeverity UmiRepositoryInspectionSeverity;
#define UMI_REPOSITORY_INSPECTION_INFO UMI_REPOSITORY_DOCTOR_INFO
#define UMI_REPOSITORY_INSPECTION_WARNING UMI_REPOSITORY_DOCTOR_WARNING
#define UMI_REPOSITORY_INSPECTION_ERROR UMI_REPOSITORY_DOCTOR_ERROR

typedef struct UmiRepositoryInspectionContext {
    const char *git_program;
    const char *repository_root;
    uint32_t timeout_ms;
} UmiRepositoryInspectionContext;

typedef struct UmiRepositoryInspectionIdentity {
    char root[UMI_REPOSITORY_INSPECTION_TEXT_CAPACITY];
    char head[128];
    char exact_tag[256];
    int bare;
    int inside_worktree;
    int shallow;
} UmiRepositoryInspectionIdentity;

typedef struct UmiRepositoryInspectionSnapshot {
    UmiRepositoryStatusSummary maintenance;
    UmiRepositoryInspectionIdentity identity;
    size_t tag_count_at_head;
    size_t configured_submodules;
    size_t tracked_gitlinks;
    int has_gitmodules;
    int has_gitignore;
    int has_gitattributes;
    int has_superproject;
    int upstream_reachable_locally;
    uint64_t revision;
} UmiRepositoryInspectionSnapshot;

/* Initialise a read-only inspection context with safe defaults. */
void umi_repository_inspection_context_init(
    UmiRepositoryInspectionContext *context,
    const char *repository_root);

/* Initialise a complete inspection snapshot before probes populate it. */
void umi_repository_inspection_snapshot_init(
    UmiRepositoryInspectionSnapshot *snapshot);

/* Copy bounded text without exposing product-specific string helpers. */
UmiStatus umi_repository_inspection_copy_text(
    char *destination,
    size_t capacity,
    const char *source);

#ifdef __cplusplus
}
#endif
#endif
