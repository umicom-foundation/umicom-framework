/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/gitlink.h
 *
 * PURPOSE:
 *   Parse and validate Git index mode-160000 gitlink records.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_GITLINK_H
#define UMICOM_REPOSITORY_GITLINK_H
#include "umicom/repository/control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the repository gitlink data shared with callers of this public contract.
 */
typedef struct UmiRepositoryGitlink {
    char path[UMI_REPOSITORY_CONTROL_PATH_CAPACITY];
    char sha[UMI_REPOSITORY_CONTROL_SHA_CAPACITY];
    unsigned stage;
    int valid;
} UmiRepositoryGitlink;
/**
 * Read repository gitlink into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_repository_gitlink_parse(
    const char *ls_files_stage_line,
    UmiRepositoryGitlink *out_gitlink);
#ifdef __cplusplus
}
#endif
#endif
