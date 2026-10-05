/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_layout.c
 *
 * PURPOSE:
 *   Describe a named editor-area layout and its group membership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_layout.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise editor wb editor layout from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_editor_layout_init(UmiEditorWbEditorLayout *s,const char *id,const char *parent){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_editor_wb_id_valid(id)||parent==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(s->id,sizeof s->id,id)!=UMI_STATUS_OK||umi_editor_wb_copy_text(s->parent_id,sizeof s->parent_id,parent)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->revision=1U;return UMI_STATUS_OK;} UmiStatus umi_editor_wb_editor_layout_set_count(UmiEditorWbEditorLayout *s,size_t count,size_t active){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||(count>0U&&active>=count))return UMI_STATUS_INVALID_ARGUMENT;s->item_count=count;s->active_index=count==0U?0U:active;s->active=count>0U;s->revision++;return UMI_STATUS_OK;} int umi_editor_wb_editor_layout_valid(const UmiEditorWbEditorLayout *s){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (s == NULL) return 0;
    if (memchr(s->id, '\0', sizeof(s->id)) == NULL) return 0;
    if (memchr(s->parent_id, '\0', sizeof(s->parent_id)) == NULL) return 0;
return s!=NULL&&umi_editor_wb_id_valid(s->id)&&s->revision>0U&&(s->item_count==0U||s->active_index<s->item_count);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiEditorWbEditorLayoutArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x67ee321a16bac2b2);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorWbEditorLayout *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorWbEditorLayout *)0)->parent_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiEditorWbEditorLayoutArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiEditorWbEditorLayout *)0)->id) - 1U +
        8U + sizeof(((UmiEditorWbEditorLayout *)0)->parent_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiEditorWbEditorLayoutArchiveWrite(UmiArchiveWriter *writer, const UmiEditorWbEditorLayout *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->parent_id, sizeof(value->parent_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active_index);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiEditorWbEditorLayoutArchiveRead(UmiArchiveReader *reader, UmiEditorWbEditorLayout *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->parent_id, sizeof(value->parent_id));
    value->item_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->active_index = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiEditorWbEditorLayoutArchiveValidate(const UmiEditorWbEditorLayout *value)
{
    return umi_editor_wb_editor_layout_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_editor_wb_editor_layout_archive_encode, umi_editor_wb_editor_layout_archive_decode,
    UmiEditorWbEditorLayout, UmiEditorWbEditorLayoutArchiveSchema, UmiEditorWbEditorLayoutArchiveBound, UmiEditorWbEditorLayoutArchiveWrite, UmiEditorWbEditorLayoutArchiveRead, UmiEditorWbEditorLayoutArchiveValidate)
