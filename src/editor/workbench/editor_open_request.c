/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_open_request.c
 *
 * PURPOSE:
 *   Represent a governed request to open a resource in an editor group.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_open_request.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise editor wb editor open request from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_editor_open_request_init(UmiEditorWbEditorOpenRequest *s,const char *resource,const char *group){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||resource==NULL||resource[0]=='\0'||!umi_editor_wb_id_valid(group))return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(s->resource,sizeof s->resource,resource)!=UMI_STATUS_OK||umi_editor_wb_copy_text(s->group_id,sizeof s->group_id,group)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->mode=UMI_EDITOR_WB_OPEN_NORMAL;return UMI_STATUS_OK;} int umi_editor_wb_editor_open_request_valid(const UmiEditorWbEditorOpenRequest *s){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (s == NULL) return 0;
    if (memchr(s->resource, '\0', sizeof(s->resource)) == NULL) return 0;
    if (memchr(s->group_id, '\0', sizeof(s->group_id)) == NULL) return 0;
return s!=NULL&&s->resource[0]!='\0'&&umi_editor_wb_id_valid(s->group_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiEditorWbEditorOpenRequestArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xcc85ca35bc97467a);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorWbEditorOpenRequest *)0)->resource)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorWbEditorOpenRequest *)0)->group_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiEditorWbEditorOpenRequestArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiEditorWbEditorOpenRequest *)0)->resource) - 1U +
        8U + sizeof(((UmiEditorWbEditorOpenRequest *)0)->group_id) - 1U +
        8U +
        8U;
}
static void UmiEditorWbEditorOpenRequestArchiveWrite(UmiArchiveWriter *writer, const UmiEditorWbEditorOpenRequest *value)
{
    UmiArchiveWriteText(writer, value->resource, sizeof(value->resource));
    UmiArchiveWriteText(writer, value->group_id, sizeof(value->group_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->mode);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->force);
}
static void UmiEditorWbEditorOpenRequestArchiveRead(UmiArchiveReader *reader, UmiEditorWbEditorOpenRequest *value)
{
    UmiArchiveReadText(reader, value->resource, sizeof(value->resource));
    UmiArchiveReadText(reader, value->group_id, sizeof(value->group_id));
    value->mode = (UmiEditorWbOpenMode)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->force = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiEditorWbEditorOpenRequestArchiveValidate(const UmiEditorWbEditorOpenRequest *value)
{
    return umi_editor_wb_editor_open_request_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_editor_wb_editor_open_request_archive_encode, umi_editor_wb_editor_open_request_archive_decode,
    UmiEditorWbEditorOpenRequest, UmiEditorWbEditorOpenRequestArchiveSchema, UmiEditorWbEditorOpenRequestArchiveBound, UmiEditorWbEditorOpenRequestArchiveWrite, UmiEditorWbEditorOpenRequestArchiveRead, UmiEditorWbEditorOpenRequestArchiveValidate)
