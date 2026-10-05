/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/row_descriptor.c
 *
 * PURPOSE:
 *   Describe a stable enterprise row identity, display label and revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/row_descriptor.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise ui ent row descriptor from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_ent_row_descriptor_init(UmiUiEntRowDescriptor *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->row_key=0;value->label[0]='\0';value->selectable=0;value->editable=0;value->enabled=0;value->revision=0;value->selectable=1;value->enabled=1;return UMI_STATUS_OK;}
/*
 * Check that ui ent row descriptor satisfies its contract before another service relies on
 * it.
 */
int umi_ui_ent_row_descriptor_validate(const UmiUiEntRowDescriptor *value){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->label, '\0', sizeof(value->label)) == NULL) return 0;
return value!=NULL&&value->row_key!=0U;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiEntRowDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfe7875922c22e161);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntRowDescriptor *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiEntRowDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiUiEntRowDescriptor *)0)->label) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiEntRowDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiUiEntRowDescriptor *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->row_key);
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteSigned(writer, (int64_t)value->selectable);
    UmiArchiveWriteSigned(writer, (int64_t)value->editable);
    UmiArchiveWriteSigned(writer, (int64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiUiEntRowDescriptorArchiveRead(UmiArchiveReader *reader, UmiUiEntRowDescriptor *value)
{
    value->row_key = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->selectable = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->editable = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiUiEntRowDescriptorArchiveValidate(const UmiUiEntRowDescriptor *value)
{
    return umi_ui_ent_row_descriptor_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_ent_row_descriptor_archive_encode, umi_ui_ent_row_descriptor_archive_decode,
    UmiUiEntRowDescriptor, UmiUiEntRowDescriptorArchiveSchema, UmiUiEntRowDescriptorArchiveBound, UmiUiEntRowDescriptorArchiveWrite, UmiUiEntRowDescriptorArchiveRead, UmiUiEntRowDescriptorArchiveValidate)
