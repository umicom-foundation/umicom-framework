/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/workspace.h
 *
 * PURPOSE:
 *   Publish the public workspace contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_WORKSPACE_H
#define UMICOM_PLATFORM_WORKSPACE_H

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workspace data shared with callers of this public contract.
 */
typedef struct UmiWorkspace {
    char root[512];
    int trusted;
} UmiWorkspace;

/**
 * Provide the workspace open operation used by this module and its client applications.
 */
UmiStatus umi_workspace_open(UmiWorkspace *workspace, const char *root);
/**
 * Provide the workspace set trusted operation used by this module and its client
 * applications.
 */
void umi_workspace_set_trusted(UmiWorkspace *workspace, int trusted);
/**
 * Provide the workspace is trusted operation used by this module and its client
 * applications.
 */
int umi_workspace_is_trusted(const UmiWorkspace *workspace);

#ifdef __cplusplus
}
#endif

#endif
