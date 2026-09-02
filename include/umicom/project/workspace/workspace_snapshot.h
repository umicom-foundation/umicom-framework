/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_snapshot.h
 *
 * PURPOSE:
 *   Publish the public workspace snapshot contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_SNAPSHOT_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_SNAPSHOT_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace workspace snapshot data shared with callers of this
 * public contract.
 */
    typedef struct UmiProjectWorkspaceWorkspaceSnapshot {
        char workspace_id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        size_t root_count;
        size_t project_count;
        size_t ready_project_count;
        size_t blocked_project_count;
        UmiProjectWorkspaceState state;
        uint64_t revision;
    }
    UmiProjectWorkspaceWorkspaceSnapshot;
    UmiStatus umi_project_workspace_workspace_snapshot_init(UmiProjectWorkspaceWorkspaceSnapshot*value,const char*workspace_id);
    void umi_project_workspace_workspace_snapshot_recalculate(UmiProjectWorkspaceWorkspaceSnapshot*value);
#ifdef __cplusplus
}
#endif
#endif
