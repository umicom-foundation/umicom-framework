/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/aggregation_descriptor.c
 *
 * PURPOSE:
 *   Describe one summary aggregation over a column.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/aggregation_descriptor.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise ui ent aggregation descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_aggregation_descriptor_init(UmiUiEntAggregationDescriptor *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->aggregation_id[0]='\0';value->column_id[0]='\0';value->kind=0;return UMI_STATUS_OK;}
/*
 * Check that ui ent aggregation descriptor satisfies its contract before another service
 * relies on it.
 */
int umi_ui_ent_aggregation_descriptor_validate(const UmiUiEntAggregationDescriptor *value){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->aggregation_id, '\0', sizeof(value->aggregation_id)) == NULL) return 0;
    if (memchr(value->column_id, '\0', sizeof(value->column_id)) == NULL) return 0;
return value!=NULL&&umi_ui_ent_id_valid(value->aggregation_id)&&umi_ui_ent_id_valid(value->column_id)&&value->kind>=UMI_UI_ENT_AGG_COUNT&&value->kind<=UMI_UI_ENT_AGG_AVERAGE;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiEntAggregationDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdcf6c6bb7cf6594d);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntAggregationDescriptor *)0)->aggregation_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUiEntAggregationDescriptor *)0)->column_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUiEntAggregationDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUiEntAggregationDescriptor *)0)->aggregation_id) - 1U +
        8U + sizeof(((UmiUiEntAggregationDescriptor *)0)->column_id) - 1U +
        8U;
}
static void UmiUiEntAggregationDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiUiEntAggregationDescriptor *value)
{
    UmiArchiveWriteText(writer, value->aggregation_id, sizeof(value->aggregation_id));
    UmiArchiveWriteText(writer, value->column_id, sizeof(value->column_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
}
static void UmiUiEntAggregationDescriptorArchiveRead(UmiArchiveReader *reader, UmiUiEntAggregationDescriptor *value)
{
    UmiArchiveReadText(reader, value->aggregation_id, sizeof(value->aggregation_id));
    UmiArchiveReadText(reader, value->column_id, sizeof(value->column_id));
    value->kind = (UmiUiEntAggregateKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiUiEntAggregationDescriptorArchiveValidate(const UmiUiEntAggregationDescriptor *value)
{
    return umi_ui_ent_aggregation_descriptor_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_ent_aggregation_descriptor_archive_encode, umi_ui_ent_aggregation_descriptor_archive_decode,
    UmiUiEntAggregationDescriptor, UmiUiEntAggregationDescriptorArchiveSchema, UmiUiEntAggregationDescriptorArchiveBound, UmiUiEntAggregationDescriptorArchiveWrite, UmiUiEntAggregationDescriptorArchiveRead, UmiUiEntAggregationDescriptorArchiveValidate)
