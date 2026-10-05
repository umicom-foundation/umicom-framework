/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/inspector_section.c
 *
 * PURPOSE:
 *   Describe a collapsible property-inspector section.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/inspector_section.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise ui ent inspector section from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_inspector_section_init(UmiUiEntInspectorSection *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->section_id[0]='\0';value->label[0]='\0';value->order=0;value->collapsed=0;return UMI_STATUS_OK;}
/*
 * Check that ui ent inspector section satisfies its contract before another service relies
 * on it.
 */
int umi_ui_ent_inspector_section_validate(const UmiUiEntInspectorSection *value){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->section_id, '\0', sizeof(value->section_id)) == NULL) return 0;
    if (memchr(value->label, '\0', sizeof(value->label)) == NULL) return 0;
return value!=NULL&&umi_ui_ent_id_valid(value->section_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiEntInspectorSectionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x53713bbeb49a3371);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntInspectorSection *)0)->section_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntInspectorSection *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiEntInspectorSectionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiEntInspectorSection *)0)->section_id) - 1U +
        8U + sizeof(((UmiUiEntInspectorSection *)0)->label) - 1U +
        8U +
        8U;
}
static void UmiUiEntInspectorSectionArchiveWrite(UmiArchiveWriter *writer, const UmiUiEntInspectorSection *value)
{
    UmiArchiveWriteText(writer, value->section_id, sizeof(value->section_id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteSigned(writer, (int64_t)value->order);
    UmiArchiveWriteSigned(writer, (int64_t)value->collapsed);
}
static void UmiUiEntInspectorSectionArchiveRead(UmiArchiveReader *reader, UmiUiEntInspectorSection *value)
{
    UmiArchiveReadText(reader, value->section_id, sizeof(value->section_id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->order = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->collapsed = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiUiEntInspectorSectionArchiveValidate(const UmiUiEntInspectorSection *value)
{
    return umi_ui_ent_inspector_section_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_ent_inspector_section_archive_encode, umi_ui_ent_inspector_section_archive_decode,
    UmiUiEntInspectorSection, UmiUiEntInspectorSectionArchiveSchema, UmiUiEntInspectorSectionArchiveBound, UmiUiEntInspectorSectionArchiveWrite, UmiUiEntInspectorSectionArchiveRead, UmiUiEntInspectorSectionArchiveValidate)
