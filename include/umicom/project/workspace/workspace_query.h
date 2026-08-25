/* Umicom Framework | Workspace Query | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_QUERY_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_QUERY_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceWorkspaceQuery {
        char id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char topic[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char payload[UMI_PROJECT_WORKSPACE_TEXT_CAPACITY];
        uint64_t sequence;
    }
    UmiProjectWorkspaceWorkspaceQuery;
    UmiStatus umi_project_workspace_workspace_query_init(UmiProjectWorkspaceWorkspaceQuery *value,const char *id,const char *topic,const char *payload);
    UmiStatus umi_project_workspace_workspace_query_validate(const UmiProjectWorkspaceWorkspaceQuery *value);
#ifdef __cplusplus
}
#endif
#endif
