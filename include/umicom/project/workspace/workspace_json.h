/* Umicom Framework | Workspace JSON projection | MIT */
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_JSON_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_JSON_H
#include "umicom/project/workspace/workspace_snapshot.h"
#ifdef __cplusplus
extern "C" {
#endif
    UmiStatus umi_project_workspace_workspace_json_render(const UmiProjectWorkspaceWorkspaceSnapshot*value,char*out_json,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
