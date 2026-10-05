/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/zoom.c
 *
 * PURPOSE:
 *   Provide bounded zoom policy for visual authoring surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/zoom.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer zoom from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_zoom_init(UmiRadZoomPolicy *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->minimum = 0.25;
    item->maximum = 4.0;
    item->current = 4.0;
    return UMI_STATUS_OK;
}
/* Check that visual designer zoom satisfies its contract before another service relies on it. */
int umi_rad_zoom_is_valid(const UmiRadZoomPolicy *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->minimum > 0.0 && item->maximum >= item->minimum && item->current >= item->minimum && item->current <= item->maximum;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadZoomPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x040d5cc546525115);

    return schema;
}
static size_t UmiRadZoomPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiRadZoomPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiRadZoomPolicy *value)
{
    UmiArchiveWriteDouble(writer, value->minimum);
    UmiArchiveWriteDouble(writer, value->maximum);
    UmiArchiveWriteDouble(writer, value->current);
}
static void UmiRadZoomPolicyArchiveRead(UmiArchiveReader *reader, UmiRadZoomPolicy *value)
{
    value->minimum = UmiArchiveReadDouble(reader);
    value->maximum = UmiArchiveReadDouble(reader);
    value->current = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiRadZoomPolicyArchiveValidate(const UmiRadZoomPolicy *value)
{
    return umi_rad_zoom_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_zoom_archive_encode, umi_rad_zoom_archive_decode,
    UmiRadZoomPolicy, UmiRadZoomPolicyArchiveSchema, UmiRadZoomPolicyArchiveBound, UmiRadZoomPolicyArchiveWrite, UmiRadZoomPolicyArchiveRead, UmiRadZoomPolicyArchiveValidate)
