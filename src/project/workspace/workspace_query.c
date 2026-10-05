/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/workspace_query.c
 *
 * PURPOSE:
 *   Implement the workspace query behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Workspace Query | MIT */
#include "umicom/project/workspace/workspace_query.h"
#include "../../base/value_archive_internal.h"
#include "internal.h"
#include <string.h>
/*
 * Initialise project workspace workspace query from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_project_workspace_workspace_query_init(UmiProjectWorkspaceWorkspaceQuery *value,const char *id,const char *topic,const char *payload) {
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||id==NULL||topic==NULL||payload==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(value,0,sizeof(*value));
    s=umi_pw_copy(value->id,sizeof(value->id),id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s!=UMI_STATUS_OK)return s;
    s=umi_pw_copy(value->topic,sizeof(value->topic),topic);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s!=UMI_STATUS_OK)return s;
    s=umi_pw_copy(value->payload,sizeof(value->payload),payload);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s!=UMI_STATUS_OK)return s;
    value->sequence=1U;
    return umi_project_workspace_workspace_query_validate(value);
}
/*
 * Check that project workspace workspace query satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_project_workspace_workspace_query_validate(const UmiProjectWorkspaceWorkspaceQuery *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->topic, '\0', sizeof(value->topic)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->payload, '\0', sizeof(value->payload)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||value->id[0]=='\0'||value->topic[0]=='\0'||value->sequence==0U)return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiProjectWorkspaceWorkspaceQueryArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb32eaae546a1cb2e);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectWorkspaceWorkspaceQuery *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectWorkspaceWorkspaceQuery *)0)->topic)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectWorkspaceWorkspaceQuery *)0)->payload)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiProjectWorkspaceWorkspaceQueryArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiProjectWorkspaceWorkspaceQuery *)0)->id) - 1U +
        8U + sizeof(((UmiProjectWorkspaceWorkspaceQuery *)0)->topic) - 1U +
        8U + sizeof(((UmiProjectWorkspaceWorkspaceQuery *)0)->payload) - 1U +
        8U;
}
static void UmiProjectWorkspaceWorkspaceQueryArchiveWrite(UmiArchiveWriter *writer, const UmiProjectWorkspaceWorkspaceQuery *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->topic, sizeof(value->topic));
    UmiArchiveWriteText(writer, value->payload, sizeof(value->payload));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
}
static void UmiProjectWorkspaceWorkspaceQueryArchiveRead(UmiArchiveReader *reader, UmiProjectWorkspaceWorkspaceQuery *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->topic, sizeof(value->topic));
    UmiArchiveReadText(reader, value->payload, sizeof(value->payload));
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiProjectWorkspaceWorkspaceQueryArchiveValidate(const UmiProjectWorkspaceWorkspaceQuery *value)
{
    return umi_project_workspace_workspace_query_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_project_workspace_workspace_query_archive_encode, umi_project_workspace_workspace_query_archive_decode,
    UmiProjectWorkspaceWorkspaceQuery, UmiProjectWorkspaceWorkspaceQueryArchiveSchema, UmiProjectWorkspaceWorkspaceQueryArchiveBound, UmiProjectWorkspaceWorkspaceQueryArchiveWrite, UmiProjectWorkspaceWorkspaceQueryArchiveRead, UmiProjectWorkspaceWorkspaceQueryArchiveValidate)
