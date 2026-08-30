/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/status_record.h
 *
 * PURPOSE:
 *   Represent one parsed porcelain-v2 path record.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_STATUS_RECORD_H
#define INCLUDE_UMICOM_REPOSITORY_STATUS_RECORD_H
#include "umicom/repository/inspection_types.h"
#include "umicom/repository/branch_status.h"
#include "umicom/repository/worktree_status.h"
#include "umicom/repository/index_status.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiRepositoryStatusRecordKind {
    UMI_REPOSITORY_STATUS_ORDINARY = 1,
    UMI_REPOSITORY_STATUS_RENAMED = 2,
    UMI_REPOSITORY_STATUS_UNMERGED = 3,
    UMI_REPOSITORY_STATUS_UNTRACKED = 4,
    UMI_REPOSITORY_STATUS_IGNORED = 5
} UmiRepositoryStatusRecordKind;
typedef struct UmiRepositoryStatusRecord {
    UmiRepositoryStatusRecordKind kind;
    char index_state;
    char worktree_state;
    int submodule_modified;
    char path[UMI_REPOSITORY_INSPECTION_TEXT_CAPACITY];
} UmiRepositoryStatusRecord;
/* Parse one porcelain-v2 path record without interpreting branch headers. */
UmiStatus umi_repository_status_record_parse(const char *line, UmiRepositoryStatusRecord *out_record);

#ifdef __cplusplus
}
#endif
#endif
