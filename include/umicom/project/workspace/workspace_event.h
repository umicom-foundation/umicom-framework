/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_event.h
 *
 * PURPOSE:
 *   Publish the public workspace event contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_EVENT_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_EVENT_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceWorkspaceEvent {
        char id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char topic[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char payload[UMI_PROJECT_WORKSPACE_TEXT_CAPACITY];
        uint64_t sequence;
    }
    UmiProjectWorkspaceWorkspaceEvent;
    UmiStatus umi_project_workspace_workspace_event_init(UmiProjectWorkspaceWorkspaceEvent *value,const char *id,const char *topic,const char *payload);
    UmiStatus umi_project_workspace_workspace_event_validate(const UmiProjectWorkspaceWorkspaceEvent *value);
#ifdef __cplusplus
}
#endif
#endif
