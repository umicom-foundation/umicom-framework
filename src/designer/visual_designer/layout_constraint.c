/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/layout_constraint.c
 *
 * PURPOSE:
 *   Describe minimum/maximum geometry constraints for designer components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/layout_constraint.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer layout constraint from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_layout_constraint_init(UmiRadLayoutConstraint *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->min_width = 100;
    item->max_width = 100;
    item->min_height = 100;
    item->max_height = 100;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer layout constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_layout_constraint_is_valid(const UmiRadLayoutConstraint *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->min_width >= 0 && item->min_height >= 0 && item->max_width >= item->min_width && item->max_height >= item->min_height;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadLayoutConstraintArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb8c94af80980bd13);

    return schema;
}
static size_t UmiRadLayoutConstraintArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadLayoutConstraintArchiveWrite(UmiArchiveWriter *writer, const UmiRadLayoutConstraint *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->min_width);
    UmiArchiveWriteSigned(writer, (int64_t)value->max_width);
    UmiArchiveWriteSigned(writer, (int64_t)value->min_height);
    UmiArchiveWriteSigned(writer, (int64_t)value->max_height);
}
static void UmiRadLayoutConstraintArchiveRead(UmiArchiveReader *reader, UmiRadLayoutConstraint *value)
{
    value->min_width = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->max_width = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->min_height = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->max_height = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
}
static UmiStatus UmiRadLayoutConstraintArchiveValidate(const UmiRadLayoutConstraint *value)
{
    return umi_rad_layout_constraint_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_layout_constraint_archive_encode, umi_rad_layout_constraint_archive_decode,
    UmiRadLayoutConstraint, UmiRadLayoutConstraintArchiveSchema, UmiRadLayoutConstraintArchiveBound, UmiRadLayoutConstraintArchiveWrite, UmiRadLayoutConstraintArchiveRead, UmiRadLayoutConstraintArchiveValidate)
