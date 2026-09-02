/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/status_record.c
 *
 * PURPOSE:
 *   Implement one parsed porcelain-v2 path record.
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

#include "umicom/repository/status_record.h"

#include <string.h>
/* Parse the stable record prefix and preserve the complete path tail for diagnostics. */
UmiStatus umi_repository_status_record_parse(const char *line, UmiRepositoryStatusRecord *out_record)
{
    const char *path;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (line == NULL || out_record == NULL || line[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_record, 0, sizeof(*out_record));
    /* Apply this branch only when its contract condition is satisfied. */
    if (line[0] == '?') {
        out_record->kind = UMI_REPOSITORY_STATUS_UNTRACKED;
        out_record->index_state = '.'; out_record->worktree_state = '?';
        path = line + 2;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (line[0] == '!') {
        out_record->kind = UMI_REPOSITORY_STATUS_IGNORED;
        out_record->index_state = '.'; out_record->worktree_state = '!';
        path = line + 2;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (line[0] == 'u') {
        out_record->kind = UMI_REPOSITORY_STATUS_UNMERGED;
        out_record->index_state = 'U'; out_record->worktree_state = 'U';
        path = strrchr(line, ' '); path = path != NULL ? path + 1 : "";
    } else /* Apply this branch only when its contract condition is satisfied. */ if (line[0] == '1' || line[0] == '2') {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (strlen(line) < 5U) return UMI_STATUS_PARSE_ERROR;
        out_record->kind = line[0] == '1' ? UMI_REPOSITORY_STATUS_ORDINARY : UMI_REPOSITORY_STATUS_RENAMED;
        out_record->index_state = line[2]; out_record->worktree_state = line[3];
        out_record->submodule_modified = strstr(line, " S.") != NULL || strstr(line, " SM") != NULL;
        path = strrchr(line, ' '); path = path != NULL ? path + 1 : "";
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return UMI_STATUS_PARSE_ERROR;
    }
    return umi_repository_inspection_copy_text(out_record->path, sizeof(out_record->path), path);
}

