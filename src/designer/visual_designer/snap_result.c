/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/snap_result.c
 *
 * PURPOSE:
 *   Record the deterministic outcome of a snap calculation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/snap_result.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer snap result from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_snap_result_init(UmiRadSnapResult *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);

    return UMI_STATUS_OK;
}
/* Check that visual designer snap result satisfies its contract before another service relies on it. */
int umi_rad_snap_result_is_valid(const UmiRadSnapResult *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return 1;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadSnapResultArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x146e9270e72c76d0);

    return schema;
}
static size_t UmiRadSnapResultArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadSnapResultArchiveWrite(UmiArchiveWriter *writer, const UmiRadSnapResult *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->requested.x);
    UmiArchiveWriteSigned(writer, (int64_t)value->requested.y);
    UmiArchiveWriteSigned(writer, (int64_t)value->resolved.x);
    UmiArchiveWriteSigned(writer, (int64_t)value->resolved.y);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->snapped_x);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->snapped_y);
}
static void UmiRadSnapResultArchiveRead(UmiArchiveReader *reader, UmiRadSnapResult *value)
{
    value->requested.x = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->requested.y = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->resolved.x = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->resolved.y = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->snapped_x = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->snapped_y = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadSnapResultArchiveValidate(const UmiRadSnapResult *value)
{
    return umi_rad_snap_result_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_snap_result_archive_encode, umi_rad_snap_result_archive_decode,
    UmiRadSnapResult, UmiRadSnapResultArchiveSchema, UmiRadSnapResultArchiveBound, UmiRadSnapResultArchiveWrite, UmiRadSnapResultArchiveRead, UmiRadSnapResultArchiveValidate)
