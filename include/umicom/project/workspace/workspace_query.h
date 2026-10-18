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
    /** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
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

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiProjectWorkspaceWorkspaceQuery object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_project_workspace_workspace_query_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceWorkspaceQuery *workspace, UmiProjectWorkspaceWorkspaceQuery *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_project_workspace_workspace_query_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiProjectWorkspaceWorkspaceQuery *expected, UmiProjectWorkspaceWorkspaceQuery *value);


#ifdef __cplusplus
}
#endif
#endif
