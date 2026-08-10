/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/types.c
 *
 * PURPOSE:
 *   Implement stable version-control change-state text.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/types.h"

const char *umi_vcs_change_state_text(UmiVcsChangeState state)
{
    switch (state) {
        case UMI_VCS_CHANGE_UNMODIFIED: return "unmodified";
        case UMI_VCS_CHANGE_ADDED: return "added";
        case UMI_VCS_CHANGE_MODIFIED: return "modified";
        case UMI_VCS_CHANGE_DELETED: return "deleted";
        case UMI_VCS_CHANGE_RENAMED: return "renamed";
        case UMI_VCS_CHANGE_COPIED: return "copied";
        case UMI_VCS_CHANGE_UNTRACKED: return "untracked";
        case UMI_VCS_CHANGE_IGNORED: return "ignored";
        case UMI_VCS_CHANGE_CONFLICTED: return "conflicted";
        default: return "unknown";
    }
}
