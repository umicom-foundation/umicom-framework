/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/grid_constraint.c
 *
 * PURPOSE:
 *   Describe renderer-neutral grid row/column placement and spans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/grid_constraint.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer grid constraint from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_grid_constraint_init(UmiRadGridConstraint *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->row_span = 1;
    item->column_span = 1;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer grid constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_grid_constraint_is_valid(const UmiRadGridConstraint *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->row >= 0 && item->column >= 0 && item->row_span > 0 && item->column_span > 0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadGridConstraintArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xee1d36ee6ce80f11);

    return schema;
}
static size_t UmiRadGridConstraintArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadGridConstraintArchiveWrite(UmiArchiveWriter *writer, const UmiRadGridConstraint *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->row);
    UmiArchiveWriteSigned(writer, (int64_t)value->column);
    UmiArchiveWriteSigned(writer, (int64_t)value->row_span);
    UmiArchiveWriteSigned(writer, (int64_t)value->column_span);
}
static void UmiRadGridConstraintArchiveRead(UmiArchiveReader *reader, UmiRadGridConstraint *value)
{
    value->row = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->column = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->row_span = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->column_span = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
}
static UmiStatus UmiRadGridConstraintArchiveValidate(const UmiRadGridConstraint *value)
{
    return umi_rad_grid_constraint_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_grid_constraint_archive_encode, umi_rad_grid_constraint_archive_decode,
    UmiRadGridConstraint, UmiRadGridConstraintArchiveSchema, UmiRadGridConstraintArchiveBound, UmiRadGridConstraintArchiveWrite, UmiRadGridConstraintArchiveRead, UmiRadGridConstraintArchiveValidate)
