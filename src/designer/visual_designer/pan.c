/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/pan.c
 *
 * PURPOSE:
 *   Provide deterministic canvas panning state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/pan.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer pan from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_pan_init(UmiRadPanState *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);

    return UMI_STATUS_OK;
}
/* Check that visual designer pan satisfies its contract before another service relies on it. */
int umi_rad_pan_is_valid(const UmiRadPanState *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return 1;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadPanStateArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5dfcbf0840b7a2e0);

    return schema;
}
static size_t UmiRadPanStateArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiRadPanStateArchiveWrite(UmiArchiveWriter *writer, const UmiRadPanState *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->offset.x);
    UmiArchiveWriteSigned(writer, (int64_t)value->offset.y);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiRadPanStateArchiveRead(UmiArchiveReader *reader, UmiRadPanState *value)
{
    value->offset.x = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->offset.y = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadPanStateArchiveValidate(const UmiRadPanState *value)
{
    return umi_rad_pan_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_pan_archive_encode, umi_rad_pan_archive_decode,
    UmiRadPanState, UmiRadPanStateArchiveSchema, UmiRadPanStateArchiveBound, UmiRadPanStateArchiveWrite, UmiRadPanStateArchiveRead, UmiRadPanStateArchiveValidate)
