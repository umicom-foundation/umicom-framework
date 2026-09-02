/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/workspace.c
 *
 * PURPOSE:
 *   Implement the workspace behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/workspace.h"

#include <stdio.h>
#include <string.h>

/* Provide the workspace open operation used by this module and its client applications. */
UmiStatus umi_workspace_open(UmiWorkspace *workspace, const char *root)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (workspace == 0 || root == 0 || root[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    memset(workspace, 0, sizeof(*workspace));
    (void)snprintf(workspace->root, sizeof(workspace->root), "%s", root);
    workspace->trusted = 0;
    return UMI_STATUS_OK;
}
/*
 * Provide the workspace set trusted operation used by this module and its client
 * applications.
 */
void umi_workspace_set_trusted(UmiWorkspace *workspace, int trusted)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (workspace != 0) workspace->trusted = trusted != 0;
}
/*
 * Provide the workspace is trusted operation used by this module and its client
 * applications.
 */
int umi_workspace_is_trusted(const UmiWorkspace *workspace)
{
    return workspace != 0 && workspace->trusted;
}
