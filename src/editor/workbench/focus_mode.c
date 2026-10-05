/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/focus_mode.c
 *
 * PURPOSE:
 *   Track focus mode while preserving the previously visible workbench regions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/focus_mode.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise editor wb focus mode from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_editor_wb_focus_mode_init(UmiEditorWbFocusMode *s,const char *id,bool enabled){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_editor_wb_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(s->item_id,sizeof s->item_id,id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->enabled=enabled;s->revision=1U;return UMI_STATUS_OK;}
/*
 * Copy editor wb focus mode into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_editor_wb_focus_mode_set(UmiEditorWbFocusMode *s,bool enabled){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->enabled=enabled;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(enabled)s->promoted=true;s->revision++;return UMI_STATUS_OK;}
/*
 * Check that editor wb focus mode satisfies its contract before another service relies on
 * it.
 */
int umi_editor_wb_focus_mode_valid(const UmiEditorWbFocusMode *s){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (s == NULL) return 0;
    if (memchr(s->item_id, '\0', sizeof(s->item_id)) == NULL) return 0;
return s!=NULL&&umi_editor_wb_id_valid(s->item_id)&&s->revision>0U;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiEditorWbFocusModeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x43dcb4c6356226c8);
    schema = (schema ^ (uint64_t)sizeof(((UmiEditorWbFocusMode *)0)->item_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiEditorWbFocusModeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiEditorWbFocusMode *)0)->item_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiEditorWbFocusModeArchiveWrite(UmiArchiveWriter *writer, const UmiEditorWbFocusMode *value)
{
    UmiArchiveWriteText(writer, value->item_id, sizeof(value->item_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->promoted);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiEditorWbFocusModeArchiveRead(UmiArchiveReader *reader, UmiEditorWbFocusMode *value)
{
    UmiArchiveReadText(reader, value->item_id, sizeof(value->item_id));
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->promoted = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiEditorWbFocusModeArchiveValidate(const UmiEditorWbFocusMode *value)
{
    return umi_editor_wb_focus_mode_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_editor_wb_focus_mode_archive_encode, umi_editor_wb_focus_mode_archive_decode,
    UmiEditorWbFocusMode, UmiEditorWbFocusModeArchiveSchema, UmiEditorWbFocusModeArchiveBound, UmiEditorWbFocusModeArchiveWrite, UmiEditorWbFocusModeArchiveRead, UmiEditorWbFocusModeArchiveValidate)
