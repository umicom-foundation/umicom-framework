/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/workspace_event.c
 *
 * PURPOSE:
 *   Implement the workspace event behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Workspace Event | MIT */
#include "../../base/conditional_archive_internal.h"
#include "umicom/project/workspace/workspace_event.h"
#include "../../base/value_construction_internal.h"
#include "../../base/value_archive_internal.h"
#include "internal.h"
#include <string.h>
/*
 * Initialise project workspace workspace event from caller-provided values so later
 * operations receive a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_project_workspace_workspace_event_init(UmiProjectWorkspaceWorkspaceEvent *value,const char *id,const char *topic,const char *payload) {
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
    return umi_project_workspace_workspace_event_validate(value);
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_project_workspace_workspace_event_init_build_value(UmiProjectWorkspaceWorkspaceEvent *value,const char *id,const char *topic,const char *payload) {
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
    return umi_project_workspace_workspace_event_validate(value);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_project_workspace_workspace_event_init, UmiProjectWorkspaceWorkspaceEvent, value,
    (UmiProjectWorkspaceWorkspaceEvent *value, const char *id, const char *topic, const char *payload),
    umi_project_workspace_workspace_event_init_build_value(candidate, id, topic, payload))

/*
 * Check that project workspace workspace event satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_project_workspace_workspace_event_validate(const UmiProjectWorkspaceWorkspaceEvent *value) {
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
static uint64_t UmiProjectWorkspaceWorkspaceEventArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5690cfdb4f100b7e);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectWorkspaceWorkspaceEvent *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectWorkspaceWorkspaceEvent *)0)->topic)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiProjectWorkspaceWorkspaceEvent *)0)->payload)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiProjectWorkspaceWorkspaceEventArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiProjectWorkspaceWorkspaceEvent *)0)->id) - 1U +
        8U + sizeof(((UmiProjectWorkspaceWorkspaceEvent *)0)->topic) - 1U +
        8U + sizeof(((UmiProjectWorkspaceWorkspaceEvent *)0)->payload) - 1U +
        8U;
}
static void UmiProjectWorkspaceWorkspaceEventArchiveWrite(UmiArchiveWriter *writer, const UmiProjectWorkspaceWorkspaceEvent *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->topic, sizeof(value->topic));
    UmiArchiveWriteText(writer, value->payload, sizeof(value->payload));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
}
static void UmiProjectWorkspaceWorkspaceEventArchiveRead(UmiArchiveReader *reader, UmiProjectWorkspaceWorkspaceEvent *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->topic, sizeof(value->topic));
    UmiArchiveReadText(reader, value->payload, sizeof(value->payload));
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiProjectWorkspaceWorkspaceEventArchiveValidate(const UmiProjectWorkspaceWorkspaceEvent *value)
{
    return umi_project_workspace_workspace_event_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_project_workspace_workspace_event_archive_encode, umi_project_workspace_workspace_event_archive_decode,
    UmiProjectWorkspaceWorkspaceEvent, UmiProjectWorkspaceWorkspaceEventArchiveSchema, UmiProjectWorkspaceWorkspaceEventArchiveBound, UmiProjectWorkspaceWorkspaceEventArchiveWrite, UmiProjectWorkspaceWorkspaceEventArchiveRead, UmiProjectWorkspaceWorkspaceEventArchiveValidate)

/* SDK callers choose staging storage independently of their thread stack.
 * Reuse this owner\'s exact schema, reader and validation before publishing
 * a destination; decoded values remain data, never execution authority. */
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_project_workspace_workspace_event_archive_decode_with_workspace, UmiProjectWorkspaceWorkspaceEvent,
    UmiProjectWorkspaceWorkspaceEventArchiveSchema, UmiProjectWorkspaceWorkspaceEventArchiveBound, UmiProjectWorkspaceWorkspaceEventArchiveRead, UmiProjectWorkspaceWorkspaceEventArchiveValidate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_project_workspace_workspace_event_archive_decode_if_unchanged, umi_project_workspace_workspace_event_archive_encode,
    UmiProjectWorkspaceWorkspaceEvent, UmiProjectWorkspaceWorkspaceEventArchiveSchema, UmiProjectWorkspaceWorkspaceEventArchiveBound, UmiProjectWorkspaceWorkspaceEventArchiveRead, UmiProjectWorkspaceWorkspaceEventArchiveValidate)
