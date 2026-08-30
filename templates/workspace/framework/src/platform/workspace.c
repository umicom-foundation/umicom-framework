/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/platform/workspace.c
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

UmiStatus umi_workspace_open(UmiWorkspace *workspace, const char *root)
{
    if (workspace == 0 || root == 0 || root[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    memset(workspace, 0, sizeof(*workspace));
    (void)snprintf(workspace->root, sizeof(workspace->root), "%s", root);
    workspace->trusted = 0;
    return UMI_STATUS_OK;
}
void umi_workspace_set_trusted(UmiWorkspace *workspace, int trusted)
{
    if (workspace != 0) workspace->trusted = trusted != 0;
}
int umi_workspace_is_trusted(const UmiWorkspace *workspace)
{
    return workspace != 0 && workspace->trusted;
}
