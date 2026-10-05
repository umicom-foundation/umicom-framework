/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/anchor_constraint.c
 *
 * PURPOSE:
 *   Describe edge anchors for adaptive layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/anchor_constraint.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer anchor constraint from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_anchor_constraint_init(UmiRadAnchorConstraint *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->margin = 1;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer anchor constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_anchor_constraint_is_valid(const UmiRadAnchorConstraint *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->margin >= 0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadAnchorConstraintArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf21f39d8e1a549fe);

    return schema;
}
static size_t UmiRadAnchorConstraintArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadAnchorConstraintArchiveWrite(UmiArchiveWriter *writer, const UmiRadAnchorConstraint *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->left);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->top);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->right);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->bottom);
    UmiArchiveWriteSigned(writer, (int64_t)value->margin);
}
static void UmiRadAnchorConstraintArchiveRead(UmiArchiveReader *reader, UmiRadAnchorConstraint *value)
{
    value->left = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->top = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->right = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->bottom = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->margin = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
}
static UmiStatus UmiRadAnchorConstraintArchiveValidate(const UmiRadAnchorConstraint *value)
{
    return umi_rad_anchor_constraint_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_anchor_constraint_archive_encode, umi_rad_anchor_constraint_archive_decode,
    UmiRadAnchorConstraint, UmiRadAnchorConstraintArchiveSchema, UmiRadAnchorConstraintArchiveBound, UmiRadAnchorConstraintArchiveWrite, UmiRadAnchorConstraintArchiveRead, UmiRadAnchorConstraintArchiveValidate)
