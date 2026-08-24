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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_GITLINK_H
#define UMICOM_REPOSITORY_GITLINK_H
#include "umicom/repository/control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRepositoryGitlink {
    char path[UMI_REPOSITORY_CONTROL_PATH_CAPACITY];
    char sha[UMI_REPOSITORY_CONTROL_SHA_CAPACITY];
    unsigned stage;
    int valid;
} UmiRepositoryGitlink;
UmiStatus umi_repository_gitlink_parse(
    const char *ls_files_stage_line,
    UmiRepositoryGitlink *out_gitlink);
#ifdef __cplusplus
}
#endif
#endif
