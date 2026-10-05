/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/inspector_property.c
 *
 * PURPOSE:
 *   Describe an editable property row for enterprise inspectors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/inspector_property.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise ui ent inspector property from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_inspector_property_init(UmiUiEntInspectorProperty *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->property_id[0]='\0';value->section_id[0]='\0';value->label[0]='\0';value->value[0]='\0';value->value_type[0]='\0';value->editable=0;value->required=0;return UMI_STATUS_OK;}
/*
 * Check that ui ent inspector property satisfies its contract before another service
 * relies on it.
 */
int umi_ui_ent_inspector_property_validate(const UmiUiEntInspectorProperty *value){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->property_id, '\0', sizeof(value->property_id)) == NULL) return 0;
    if (memchr(value->section_id, '\0', sizeof(value->section_id)) == NULL) return 0;
    if (memchr(value->label, '\0', sizeof(value->label)) == NULL) return 0;
    if (memchr(value->value, '\0', sizeof(value->value)) == NULL) return 0;
    if (memchr(value->value_type, '\0', sizeof(value->value_type)) == NULL) return 0;
return value!=NULL&&umi_ui_ent_id_valid(value->property_id)&&umi_ui_ent_id_valid(value->section_id)&&value->label[0]!='\0';}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiEntInspectorPropertyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x04a02a329d8fb516);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntInspectorProperty *)0)->property_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntInspectorProperty *)0)->section_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntInspectorProperty *)0)->label)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntInspectorProperty *)0)->value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntInspectorProperty *)0)->value_type)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiEntInspectorPropertyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiEntInspectorProperty *)0)->property_id) - 1U +
        8U + sizeof(((UmiUiEntInspectorProperty *)0)->section_id) - 1U +
        8U + sizeof(((UmiUiEntInspectorProperty *)0)->label) - 1U +
        8U + sizeof(((UmiUiEntInspectorProperty *)0)->value) - 1U +
        8U + sizeof(((UmiUiEntInspectorProperty *)0)->value_type) - 1U +
        8U +
        8U;
}
static void UmiUiEntInspectorPropertyArchiveWrite(UmiArchiveWriter *writer, const UmiUiEntInspectorProperty *value)
{
    UmiArchiveWriteText(writer, value->property_id, sizeof(value->property_id));
    UmiArchiveWriteText(writer, value->section_id, sizeof(value->section_id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteText(writer, value->value, sizeof(value->value));
    UmiArchiveWriteText(writer, value->value_type, sizeof(value->value_type));
    UmiArchiveWriteSigned(writer, (int64_t)value->editable);
    UmiArchiveWriteSigned(writer, (int64_t)value->required);
}
static void UmiUiEntInspectorPropertyArchiveRead(UmiArchiveReader *reader, UmiUiEntInspectorProperty *value)
{
    UmiArchiveReadText(reader, value->property_id, sizeof(value->property_id));
    UmiArchiveReadText(reader, value->section_id, sizeof(value->section_id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    UmiArchiveReadText(reader, value->value, sizeof(value->value));
    UmiArchiveReadText(reader, value->value_type, sizeof(value->value_type));
    value->editable = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->required = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiUiEntInspectorPropertyArchiveValidate(const UmiUiEntInspectorProperty *value)
{
    return umi_ui_ent_inspector_property_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_ent_inspector_property_archive_encode, umi_ui_ent_inspector_property_archive_decode,
    UmiUiEntInspectorProperty, UmiUiEntInspectorPropertyArchiveSchema, UmiUiEntInspectorPropertyArchiveBound, UmiUiEntInspectorPropertyArchiveWrite, UmiUiEntInspectorPropertyArchiveRead, UmiUiEntInspectorPropertyArchiveValidate)
