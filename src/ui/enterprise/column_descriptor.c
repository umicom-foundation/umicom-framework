/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/column_descriptor.c
 *
 * PURPOSE:
 *   Describe an enterprise grid column with sizing, edit and interaction capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/column_descriptor.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise ui ent column descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_column_descriptor_init(UmiUiEntColumnDescriptor *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->column_id[0]='\0';value->label[0]='\0';value->width=0;value->minimum_width=0;value->maximum_width=0;value->sortable=0;value->filterable=0;value->editable=0;value->resizable=0;value->visible=0;value->frozen=0;value->width=120;value->minimum_width=32;value->maximum_width=2048;value->sortable=1;value->filterable=1;value->resizable=1;value->visible=1;return UMI_STATUS_OK;}
/*
 * Check that ui ent column descriptor satisfies its contract before another service relies
 * on it.
 */
int umi_ui_ent_column_descriptor_validate(const UmiUiEntColumnDescriptor *value){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->column_id, '\0', sizeof(value->column_id)) == NULL) return 0;
    if (memchr(value->label, '\0', sizeof(value->label)) == NULL) return 0;
return value!=NULL&&umi_ui_ent_id_valid(value->column_id)&&value->minimum_width>0&&value->maximum_width>=value->minimum_width&&value->width>=value->minimum_width&&value->width<=value->maximum_width;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiEntColumnDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x9d74947ec1acc79d);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntColumnDescriptor *)0)->column_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntColumnDescriptor *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiEntColumnDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiEntColumnDescriptor *)0)->column_id) - 1U +
        8U + sizeof(((UmiUiEntColumnDescriptor *)0)->label) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiEntColumnDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiUiEntColumnDescriptor *value)
{
    UmiArchiveWriteText(writer, value->column_id, sizeof(value->column_id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteSigned(writer, (int64_t)value->width);
    UmiArchiveWriteSigned(writer, (int64_t)value->minimum_width);
    UmiArchiveWriteSigned(writer, (int64_t)value->maximum_width);
    UmiArchiveWriteSigned(writer, (int64_t)value->sortable);
    UmiArchiveWriteSigned(writer, (int64_t)value->filterable);
    UmiArchiveWriteSigned(writer, (int64_t)value->editable);
    UmiArchiveWriteSigned(writer, (int64_t)value->resizable);
    UmiArchiveWriteSigned(writer, (int64_t)value->visible);
    UmiArchiveWriteSigned(writer, (int64_t)value->frozen);
}
static void UmiUiEntColumnDescriptorArchiveRead(UmiArchiveReader *reader, UmiUiEntColumnDescriptor *value)
{
    UmiArchiveReadText(reader, value->column_id, sizeof(value->column_id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->width = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->minimum_width = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->maximum_width = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->sortable = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->filterable = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->editable = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->resizable = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->visible = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->frozen = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiUiEntColumnDescriptorArchiveValidate(const UmiUiEntColumnDescriptor *value)
{
    return umi_ui_ent_column_descriptor_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_ent_column_descriptor_archive_encode, umi_ui_ent_column_descriptor_archive_decode,
    UmiUiEntColumnDescriptor, UmiUiEntColumnDescriptorArchiveSchema, UmiUiEntColumnDescriptorArchiveBound, UmiUiEntColumnDescriptorArchiveWrite, UmiUiEntColumnDescriptorArchiveRead, UmiUiEntColumnDescriptorArchiveValidate)
