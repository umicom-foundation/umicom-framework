/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/gitlink.c
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
#include "umicom/repository/gitlink.h"

#include <stdio.h>
#include <string.h>

#include "umicom/repository/path.h"
#include "umicom/repository/sha.h"

UmiStatus umi_repository_gitlink_parse(
    const char *ls_files_stage_line,
    UmiRepositoryGitlink *out_gitlink)
{
    char mode[16];
    char sha[UMI_REPOSITORY_CONTROL_SHA_CAPACITY];
    unsigned stage = 0U;
    char path[UMI_REPOSITORY_CONTROL_PATH_CAPACITY];
    int parsed;
    if (ls_files_stage_line == NULL || out_gitlink == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_gitlink, 0, sizeof(*out_gitlink));
    parsed = sscanf(
        ls_files_stage_line,
        "%15s %64s %u\t%2047[^\n]",
        mode, sha, &stage, path);
    if (parsed != 4) return UMI_STATUS_PARSE_ERROR;
    if (strcmp(mode, "160000") != 0 ||
        !umi_repository_sha_is_valid(sha) ||
        !umi_repository_control_path_is_safe_relative(path)) {
        return UMI_STATUS_INVALID_STATE;
    }
    (void)snprintf(out_gitlink->path, sizeof(out_gitlink->path), "%s", path);
    (void)snprintf(out_gitlink->sha, sizeof(out_gitlink->sha), "%s", sha);
    out_gitlink->stage = stage;
    out_gitlink->valid = 1;
    return UMI_STATUS_OK;
}
