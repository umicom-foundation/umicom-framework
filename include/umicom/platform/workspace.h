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

typedef struct UmiWorkspace {
    char root[512];
    int trusted;
} UmiWorkspace;

UmiStatus umi_workspace_open(UmiWorkspace *workspace, const char *root);
void umi_workspace_set_trusted(UmiWorkspace *workspace, int trusted);
int umi_workspace_is_trusted(const UmiWorkspace *workspace);

#ifdef __cplusplus
}
#endif

#endif
