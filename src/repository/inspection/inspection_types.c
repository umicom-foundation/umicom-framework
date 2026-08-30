/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/inspection_types.c
 *
 * PURPOSE:
 *   Implement common initialisation and bounded text helpers for repository inspection.
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

#include "umicom/repository/inspection_types.h"

#include <stdio.h>
#include <string.h>

/* Initialise an operation context without requiring the full compiler toolchain. */
void umi_repository_inspection_context_init(
    UmiRepositoryInspectionContext *context,
    const char *repository_root)
{
    if (context == NULL) return;
    context->git_program = "git";
    context->repository_root =
        (repository_root != NULL && repository_root[0] != '\0')
            ? repository_root
            : ".";
    context->timeout_ms = 10000U;
}

/* Initialise all fields and reuse the established maintenance summary model. */
void umi_repository_inspection_snapshot_init(
    UmiRepositoryInspectionSnapshot *snapshot)
{
    if (snapshot == NULL) return;
    (void)memset(snapshot, 0, sizeof(*snapshot));
    umi_repository_status_summary_init(&snapshot->maintenance);
}

/* Copy one public string with explicit capacity and deterministic truncation failure. */
UmiStatus umi_repository_inspection_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    int written;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(destination, capacity, "%s", source);
    if (written < 0) return UMI_STATUS_IO_ERROR;
    if ((size_t)written >= capacity) {
        destination[capacity - 1U] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
