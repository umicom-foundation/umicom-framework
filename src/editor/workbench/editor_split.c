/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_split.c
 *
 * PURPOSE:
 *   Describe one horizontal or vertical editor split with a bounded ratio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_split.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise editor wb editor split from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_editor_split_init(UmiEditorWbEditorSplit *s,const char *id,UmiEditorWbOrientation o,double ratio){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_editor_wb_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(s->split_id,sizeof s->split_id,id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->orientation=o;return umi_editor_wb_editor_split_set_ratio(s,ratio);}
/*
 * Provide the editor wb editor split set ratio operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_wb_editor_split_set_ratio(UmiEditorWbEditorSplit *s,double ratio){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||ratio<0.1||ratio>0.9)return UMI_STATUS_INVALID_ARGUMENT;s->ratio=ratio;return UMI_STATUS_OK;}
/*
 * Check that editor wb editor split satisfies its contract before another service relies
 * on it.
 */
int umi_editor_wb_editor_split_valid(const UmiEditorWbEditorSplit *s){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (s == NULL) return 0;
    if (memchr(s->split_id, '\0', sizeof(s->split_id)) == NULL) return 0;
return s!=NULL&&umi_editor_wb_id_valid(s->split_id)&&(s->orientation==UMI_EDITOR_WB_HORIZONTAL||s->orientation==UMI_EDITOR_WB_VERTICAL)&&s->ratio>=0.1&&s->ratio<=0.9;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiEditorWbEditorSplitArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd5b5b25f2136e126);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorWbEditorSplit *)0)->split_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiEditorWbEditorSplitArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiEditorWbEditorSplit *)0)->split_id) - 1U +
        8U +
        8U;
}
static void UmiEditorWbEditorSplitArchiveWrite(UmiArchiveWriter *writer, const UmiEditorWbEditorSplit *value)
{
    UmiArchiveWriteText(writer, value->split_id, sizeof(value->split_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->orientation);
    UmiArchiveWriteDouble(writer, value->ratio);
}
static void UmiEditorWbEditorSplitArchiveRead(UmiArchiveReader *reader, UmiEditorWbEditorSplit *value)
{
    UmiArchiveReadText(reader, value->split_id, sizeof(value->split_id));
    value->orientation = (UmiEditorWbOrientation)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->ratio = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiEditorWbEditorSplitArchiveValidate(const UmiEditorWbEditorSplit *value)
{
    return umi_editor_wb_editor_split_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_editor_wb_editor_split_archive_encode, umi_editor_wb_editor_split_archive_decode,
    UmiEditorWbEditorSplit, UmiEditorWbEditorSplitArchiveSchema, UmiEditorWbEditorSplitArchiveBound, UmiEditorWbEditorSplitArchiveWrite, UmiEditorWbEditorSplitArchiveRead, UmiEditorWbEditorSplitArchiveValidate)
