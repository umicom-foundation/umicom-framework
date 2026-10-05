/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/device_profile.c
 *
 * PURPOSE:
 *   Describe preview device dimensions, density and input characteristics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/device_profile.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer device profile from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_device_profile_init(UmiRadDeviceProfile *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->profile_id, sizeof item->profile_id, "device_profile");
    item->width = 100;
    item->height = 100;
    item->dpi = 96U;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer device profile satisfies its contract before another service relies on
 * it.
 */
int umi_rad_device_profile_is_valid(const UmiRadDeviceProfile *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->profile_id, '\0', sizeof(item->profile_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->width > 0 && item->height > 0 && item->dpi > 0U;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadDeviceProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x11b9f23d84ee14f6);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadDeviceProfile *)0)->profile_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadDeviceProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadDeviceProfile *)0)->profile_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadDeviceProfileArchiveWrite(UmiArchiveWriter *writer, const UmiRadDeviceProfile *value)
{
    UmiArchiveWriteText(writer, value->profile_id, sizeof(value->profile_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->width);
    UmiArchiveWriteSigned(writer, (int64_t)value->height);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->dpi);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->touch);
}
static void UmiRadDeviceProfileArchiveRead(UmiArchiveReader *reader, UmiRadDeviceProfile *value)
{
    UmiArchiveReadText(reader, value->profile_id, sizeof(value->profile_id));
    value->width = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->height = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->dpi = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->touch = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadDeviceProfileArchiveValidate(const UmiRadDeviceProfile *value)
{
    return umi_rad_device_profile_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_device_profile_archive_encode, umi_rad_device_profile_archive_decode,
    UmiRadDeviceProfile, UmiRadDeviceProfileArchiveSchema, UmiRadDeviceProfileArchiveBound, UmiRadDeviceProfileArchiveWrite, UmiRadDeviceProfileArchiveRead, UmiRadDeviceProfileArchiveValidate)
