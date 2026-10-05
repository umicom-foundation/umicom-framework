/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/inspector_editor.c
 *
 * PURPOSE:
 *   Describe the semantic editor used for an inspector property.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/inspector_editor.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise ui ent inspector editor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_inspector_editor_init(UmiUiEntInspectorEditor *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->property_id[0]='\0';value->editor_kind[0]='\0';value->choice_count=0;value->multiline=0;value->read_only=0;return UMI_STATUS_OK;}
/*
 * Check that ui ent inspector editor satisfies its contract before another service relies
 * on it.
 */
int umi_ui_ent_inspector_editor_validate(const UmiUiEntInspectorEditor *value){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->property_id, '\0', sizeof(value->property_id)) == NULL) return 0;
    if (memchr(value->editor_kind, '\0', sizeof(value->editor_kind)) == NULL) return 0;
return value!=NULL&&umi_ui_ent_id_valid(value->property_id)&&umi_ui_ent_id_valid(value->editor_kind);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiEntInspectorEditorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8be61269f3a96f84);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntInspectorEditor *)0)->property_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntInspectorEditor *)0)->editor_kind)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiEntInspectorEditorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiEntInspectorEditor *)0)->property_id) - 1U +
        8U + sizeof(((UmiUiEntInspectorEditor *)0)->editor_kind) - 1U +
        8U +
        8U +
        8U;
}
static void UmiUiEntInspectorEditorArchiveWrite(UmiArchiveWriter *writer, const UmiUiEntInspectorEditor *value)
{
    UmiArchiveWriteText(writer, value->property_id, sizeof(value->property_id));
    UmiArchiveWriteText(writer, value->editor_kind, sizeof(value->editor_kind));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->choice_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->multiline);
    UmiArchiveWriteSigned(writer, (int64_t)value->read_only);
}
static void UmiUiEntInspectorEditorArchiveRead(UmiArchiveReader *reader, UmiUiEntInspectorEditor *value)
{
    UmiArchiveReadText(reader, value->property_id, sizeof(value->property_id));
    UmiArchiveReadText(reader, value->editor_kind, sizeof(value->editor_kind));
    value->choice_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->multiline = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->read_only = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiUiEntInspectorEditorArchiveValidate(const UmiUiEntInspectorEditor *value)
{
    return umi_ui_ent_inspector_editor_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_ent_inspector_editor_archive_encode, umi_ui_ent_inspector_editor_archive_decode,
    UmiUiEntInspectorEditor, UmiUiEntInspectorEditorArchiveSchema, UmiUiEntInspectorEditorArchiveBound, UmiUiEntInspectorEditorArchiveWrite, UmiUiEntInspectorEditorArchiveRead, UmiUiEntInspectorEditorArchiveValidate)
