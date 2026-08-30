/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/porcelain_parser.c
 *
 * PURPOSE:
 *   Implement parsing for one Git porcelain-v2 status stream into existing maintenance models.
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

#include "umicom/repository/porcelain_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/repository/git_output.h"
#include "umicom/repository/status_record.h"
/* Parse porcelain once so branch/worktree/index probes can share exactly one parser. */
UmiStatus umi_repository_porcelain_parse(const char *text, UmiRepositoryPorcelainStatus *out_status)
{
    size_t count, index;
    char line[4096];
    if (text == NULL || out_status == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_status, 0, sizeof(*out_status));
    umi_repository_branch_status_init(&out_status->branch);
    umi_repository_worktree_status_init(&out_status->worktree);
    umi_repository_index_status_init(&out_status->index);
    count = umi_repository_git_output_line_count(text);
    for (index = 0U; index < count; ++index) {
        UmiRepositoryStatusRecord record;
        if (umi_repository_git_output_line_at(text, index, line, sizeof(line)) != UMI_STATUS_OK) continue;
        if (strncmp(line, "# branch.head ", 14U) == 0) {
            (void)umi_repository_inspection_copy_text(out_status->branch.branch, sizeof(out_status->branch.branch), line + 14);
            out_status->branch.detached = strcmp(out_status->branch.branch, "(detached)") == 0;
            continue;
        }
        if (strncmp(line, "# branch.upstream ", 18U) == 0) {
            (void)umi_repository_inspection_copy_text(out_status->branch.upstream, sizeof(out_status->branch.upstream), line + 18);
            continue;
        }
        if (strncmp(line, "# branch.ab +", 13U) == 0) {
            unsigned long long ahead = 0U, behind = 0U;
            if (sscanf(line, "# branch.ab +%llu -%llu", &ahead, &behind) == 2) {
                out_status->branch.ahead = (size_t)ahead;
                out_status->branch.behind = (size_t)behind;
            }
            continue;
        }
        if (line[0] == '#') continue;
        if (umi_repository_status_record_parse(line, &record) != UMI_STATUS_OK) continue;
        if (record.kind == UMI_REPOSITORY_STATUS_UNTRACKED) ++out_status->worktree.untracked;
        if (record.kind == UMI_REPOSITORY_STATUS_UNMERGED) {
            ++out_status->worktree.conflicted; ++out_status->index.conflicted_paths;
        }
        if (record.index_state != '.' && record.index_state != '?' && record.index_state != '!') ++out_status->index.staged_paths;
        if (record.worktree_state == 'D') ++out_status->worktree.deleted;
        else if (record.worktree_state != '.' && record.worktree_state != '?' && record.worktree_state != '!') ++out_status->worktree.modified;
        if (record.submodule_modified) ++out_status->submodule_dirty;
    }
    return UMI_STATUS_OK;
}

