/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/z_order.c
 *
 * PURPOSE:
 *   Represent component stacking order independently of renderer implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/z_order.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer z order from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_z_order_init(UmiRadZOrder *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->component_id, sizeof item->component_id, "z_order");
    return UMI_STATUS_OK;
}
/* Check that visual designer z order satisfies its contract before another service relies on it. */
int umi_rad_z_order_is_valid(const UmiRadZOrder *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->component_id, '\0', sizeof(item->component_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->component_id) && item->order >= 0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadZOrderArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x90d4be8cdeb6602c);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadZOrder *)0)->component_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadZOrderArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadZOrder *)0)->component_id) - 1U +
        8U;
}
static void UmiRadZOrderArchiveWrite(UmiArchiveWriter *writer, const UmiRadZOrder *value)
{
    UmiArchiveWriteText(writer, value->component_id, sizeof(value->component_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->order);
}
static void UmiRadZOrderArchiveRead(UmiArchiveReader *reader, UmiRadZOrder *value)
{
    UmiArchiveReadText(reader, value->component_id, sizeof(value->component_id));
    value->order = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
}
static UmiStatus UmiRadZOrderArchiveValidate(const UmiRadZOrder *value)
{
    return umi_rad_z_order_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_z_order_archive_encode, umi_rad_z_order_archive_decode,
    UmiRadZOrder, UmiRadZOrderArchiveSchema, UmiRadZOrderArchiveBound, UmiRadZOrderArchiveWrite, UmiRadZOrderArchiveRead, UmiRadZOrderArchiveValidate)
