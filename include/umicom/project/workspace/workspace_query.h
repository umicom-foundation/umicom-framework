/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_query.h
 *
 * PURPOSE:
 *   Publish the public workspace query contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_QUERY_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_QUERY_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace workspace query data shared with callers of this public
 * contract.
 */
    typedef struct UmiProjectWorkspaceWorkspaceQuery {
        char id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char topic[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char payload[UMI_PROJECT_WORKSPACE_TEXT_CAPACITY];
        uint64_t sequence;
    }
    UmiProjectWorkspaceWorkspaceQuery;
    UmiStatus umi_project_workspace_workspace_query_init(UmiProjectWorkspaceWorkspaceQuery *value,const char *id,const char *topic,const char *payload);
    UmiStatus umi_project_workspace_workspace_query_validate(const UmiProjectWorkspaceWorkspaceQuery *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_workspace_query_archive_encode(const UmiProjectWorkspaceWorkspaceQuery *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_workspace_query_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceWorkspaceQuery *value);

#ifdef __cplusplus
}
#endif
#endif
