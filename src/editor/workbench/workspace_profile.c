/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/workspace_profile.c
 *
 * PURPOSE:
 *   Describe reusable editor-workspace defaults without moving them into Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/workspace_profile.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise editor wb workspace profile from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_workspace_profile_init(UmiEditorWbWorkspaceProfile *s,const char *id,const char *text){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_editor_wb_id_valid(id)||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(s->id,sizeof s->id,id)!=UMI_STATUS_OK||umi_editor_wb_copy_text(s->text,sizeof s->text,text)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;} UmiStatus umi_editor_wb_workspace_profile_set_values(UmiEditorWbWorkspaceProfile *s,uint64_t a,uint64_t b,bool e){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->primary=a;s->secondary=b;s->enabled=e;return UMI_STATUS_OK;} int umi_editor_wb_workspace_profile_valid(const UmiEditorWbWorkspaceProfile *s){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (s == NULL) return 0;
    if (memchr(s->id, '\0', sizeof(s->id)) == NULL) return 0;
    if (memchr(s->text, '\0', sizeof(s->text)) == NULL) return 0;
return s!=NULL&&umi_editor_wb_id_valid(s->id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiEditorWbWorkspaceProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x67e6fc86d68346a9);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorWbWorkspaceProfile *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorWbWorkspaceProfile *)0)->text)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiEditorWbWorkspaceProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiEditorWbWorkspaceProfile *)0)->id) - 1U +
        8U + sizeof(((UmiEditorWbWorkspaceProfile *)0)->text) - 1U +
        8U +
        8U +
        8U;
}
static void UmiEditorWbWorkspaceProfileArchiveWrite(UmiArchiveWriter *writer, const UmiEditorWbWorkspaceProfile *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->text, sizeof(value->text));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->primary);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->secondary);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiEditorWbWorkspaceProfileArchiveRead(UmiArchiveReader *reader, UmiEditorWbWorkspaceProfile *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->text, sizeof(value->text));
    value->primary = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->secondary = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiEditorWbWorkspaceProfileArchiveValidate(const UmiEditorWbWorkspaceProfile *value)
{
    return umi_editor_wb_workspace_profile_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_editor_wb_workspace_profile_archive_encode, umi_editor_wb_workspace_profile_archive_decode,
    UmiEditorWbWorkspaceProfile, UmiEditorWbWorkspaceProfileArchiveSchema, UmiEditorWbWorkspaceProfileArchiveBound, UmiEditorWbWorkspaceProfileArchiveWrite, UmiEditorWbWorkspaceProfileArchiveRead, UmiEditorWbWorkspaceProfileArchiveValidate)
