/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/snap_policy.c
 *
 * PURPOSE:
 *   Configure grid, guide and component snapping tolerance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/snap_policy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer snap policy from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_snap_policy_init(UmiRadSnapPolicy *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->tolerance = 1;
    return UMI_STATUS_OK;
}
/* Check that visual designer snap policy satisfies its contract before another service relies on it. */
int umi_rad_snap_policy_is_valid(const UmiRadSnapPolicy *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->tolerance >= 0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadSnapPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdd860938e95e43a3);

    return schema;
}
static size_t UmiRadSnapPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadSnapPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiRadSnapPolicy *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->grid_enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->guides_enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->components_enabled);
    UmiArchiveWriteSigned(writer, (int64_t)value->tolerance);
}
static void UmiRadSnapPolicyArchiveRead(UmiArchiveReader *reader, UmiRadSnapPolicy *value)
{
    value->grid_enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->guides_enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->components_enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->tolerance = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
}
static UmiStatus UmiRadSnapPolicyArchiveValidate(const UmiRadSnapPolicy *value)
{
    return umi_rad_snap_policy_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_snap_policy_archive_encode, umi_rad_snap_policy_archive_decode,
    UmiRadSnapPolicy, UmiRadSnapPolicyArchiveSchema, UmiRadSnapPolicyArchiveBound, UmiRadSnapPolicyArchiveWrite, UmiRadSnapPolicyArchiveRead, UmiRadSnapPolicyArchiveValidate)
