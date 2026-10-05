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
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace workspace event data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceWorkspaceEvent {
        char id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char topic[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char payload[UMI_PROJECT_WORKSPACE_TEXT_CAPACITY];
        uint64_t sequence;
    }
    UmiProjectWorkspaceWorkspaceEvent;
    UmiStatus umi_project_workspace_workspace_event_init(UmiProjectWorkspaceWorkspaceEvent *value,const char *id,const char *topic,const char *payload);
    UmiStatus umi_project_workspace_workspace_event_validate(const UmiProjectWorkspaceWorkspaceEvent *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_workspace_event_archive_encode(const UmiProjectWorkspaceWorkspaceEvent *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_workspace_event_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceWorkspaceEvent *value);

#ifdef __cplusplus
}
#endif
#endif
