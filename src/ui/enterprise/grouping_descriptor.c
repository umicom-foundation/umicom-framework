/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/grouping_descriptor.c
 *
 * PURPOSE:
 *   Describe a grouping key and default expansion semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/grouping_descriptor.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise ui ent grouping descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_grouping_descriptor_init(UmiUiEntGroupingDescriptor *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->column_id[0]='\0';value->level=0;value->expanded_by_default=0;return UMI_STATUS_OK;}
/*
 * Check that ui ent grouping descriptor satisfies its contract before another service
 * relies on it.
 */
int umi_ui_ent_grouping_descriptor_validate(const UmiUiEntGroupingDescriptor *value){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->column_id, '\0', sizeof(value->column_id)) == NULL) return 0;
return value!=NULL&&umi_ui_ent_id_valid(value->column_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiEntGroupingDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x035ca73e600e0533);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntGroupingDescriptor *)0)->column_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiEntGroupingDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiEntGroupingDescriptor *)0)->column_id) - 1U +
        8U +
        8U;
}
static void UmiUiEntGroupingDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiUiEntGroupingDescriptor *value)
{
    UmiArchiveWriteText(writer, value->column_id, sizeof(value->column_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->level);
    UmiArchiveWriteSigned(writer, (int64_t)value->expanded_by_default);
}
static void UmiUiEntGroupingDescriptorArchiveRead(UmiArchiveReader *reader, UmiUiEntGroupingDescriptor *value)
{
    UmiArchiveReadText(reader, value->column_id, sizeof(value->column_id));
    value->level = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->expanded_by_default = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiUiEntGroupingDescriptorArchiveValidate(const UmiUiEntGroupingDescriptor *value)
{
    return umi_ui_ent_grouping_descriptor_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_ent_grouping_descriptor_archive_encode, umi_ui_ent_grouping_descriptor_archive_decode,
    UmiUiEntGroupingDescriptor, UmiUiEntGroupingDescriptorArchiveSchema, UmiUiEntGroupingDescriptorArchiveBound, UmiUiEntGroupingDescriptorArchiveWrite, UmiUiEntGroupingDescriptorArchiveRead, UmiUiEntGroupingDescriptorArchiveValidate)
