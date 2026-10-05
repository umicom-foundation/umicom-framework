/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/sort_descriptor.c
 *
 * PURPOSE:
 *   Describe one deterministic multi-column sort key.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/sort_descriptor.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise ui ent sort descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_sort_descriptor_init(UmiUiEntSortDescriptor *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->column_id[0]='\0';value->direction=0;value->priority=0;value->case_sensitive=0;return UMI_STATUS_OK;}
/*
 * Check that ui ent sort descriptor satisfies its contract before another service relies
 * on it.
 */
int umi_ui_ent_sort_descriptor_validate(const UmiUiEntSortDescriptor *value){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->column_id, '\0', sizeof(value->column_id)) == NULL) return 0;
return value!=NULL&&umi_ui_ent_id_valid(value->column_id)&&value->direction>=UMI_UI_ENT_SORT_NONE&&value->direction<=UMI_UI_ENT_SORT_DESCENDING;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiEntSortDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x2fe026ad08b03b0b);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntSortDescriptor *)0)->column_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiEntSortDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiEntSortDescriptor *)0)->column_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiUiEntSortDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiUiEntSortDescriptor *value)
{
    UmiArchiveWriteText(writer, value->column_id, sizeof(value->column_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->direction);
    UmiArchiveWriteSigned(writer, (int64_t)value->priority);
    UmiArchiveWriteSigned(writer, (int64_t)value->case_sensitive);
}
static void UmiUiEntSortDescriptorArchiveRead(UmiArchiveReader *reader, UmiUiEntSortDescriptor *value)
{
    UmiArchiveReadText(reader, value->column_id, sizeof(value->column_id));
    value->direction = (UmiUiEntSortDirection)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->priority = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->case_sensitive = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiUiEntSortDescriptorArchiveValidate(const UmiUiEntSortDescriptor *value)
{
    return umi_ui_ent_sort_descriptor_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_ent_sort_descriptor_archive_encode, umi_ui_ent_sort_descriptor_archive_decode,
    UmiUiEntSortDescriptor, UmiUiEntSortDescriptorArchiveSchema, UmiUiEntSortDescriptorArchiveBound, UmiUiEntSortDescriptorArchiveWrite, UmiUiEntSortDescriptorArchiveRead, UmiUiEntSortDescriptorArchiveValidate)
