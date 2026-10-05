/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/flex_constraint.c
 *
 * PURPOSE:
 *   Describe renderer-neutral flexible-box growth and basis constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/flex_constraint.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer flex constraint from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_flex_constraint_init(UmiRadFlexConstraint *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->grow = 1.0;
    item->shrink = 1.0;
    item->basis = 1;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer flex constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_flex_constraint_is_valid(const UmiRadFlexConstraint *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->grow >= 0.0 && item->shrink >= 0.0 && item->basis >= 0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadFlexConstraintArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa848cf8654d22d53);

    return schema;
}
static size_t UmiRadFlexConstraintArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadFlexConstraintArchiveWrite(UmiArchiveWriter *writer, const UmiRadFlexConstraint *value)
{
    UmiArchiveWriteDouble(writer, value->grow);
    UmiArchiveWriteDouble(writer, value->shrink);
    UmiArchiveWriteSigned(writer, (int64_t)value->basis);
    UmiArchiveWriteSigned(writer, (int64_t)value->order);
}
static void UmiRadFlexConstraintArchiveRead(UmiArchiveReader *reader, UmiRadFlexConstraint *value)
{
    value->grow = UmiArchiveReadDouble(reader);
    value->shrink = UmiArchiveReadDouble(reader);
    value->basis = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->order = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
}
static UmiStatus UmiRadFlexConstraintArchiveValidate(const UmiRadFlexConstraint *value)
{
    return umi_rad_flex_constraint_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_flex_constraint_archive_encode, umi_rad_flex_constraint_archive_decode,
    UmiRadFlexConstraint, UmiRadFlexConstraintArchiveSchema, UmiRadFlexConstraintArchiveBound, UmiRadFlexConstraintArchiveWrite, UmiRadFlexConstraintArchiveRead, UmiRadFlexConstraintArchiveValidate)
