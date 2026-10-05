/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/ruler.c
 *
 * PURPOSE:
 *   Describe design-time rulers and origin offsets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/ruler.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer ruler from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_ruler_init(UmiRadRuler *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->major_step = 1;
    item->visible = true;
    return UMI_STATUS_OK;
}
/* Check that visual designer ruler satisfies its contract before another service relies on it. */
int umi_rad_ruler_is_valid(const UmiRadRuler *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->major_step > 0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadRulerArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8e10608417f7ea20);

    return schema;
}
static size_t UmiRadRulerArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadRulerArchiveWrite(UmiArchiveWriter *writer, const UmiRadRuler *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->orientation);
    UmiArchiveWriteSigned(writer, (int64_t)value->origin);
    UmiArchiveWriteSigned(writer, (int64_t)value->major_step);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->visible);
}
static void UmiRadRulerArchiveRead(UmiArchiveReader *reader, UmiRadRuler *value)
{
    value->orientation = (UmiRadOrientation)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->origin = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->major_step = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->visible = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadRulerArchiveValidate(const UmiRadRuler *value)
{
    return umi_rad_ruler_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_ruler_archive_encode, umi_rad_ruler_archive_decode,
    UmiRadRuler, UmiRadRulerArchiveSchema, UmiRadRulerArchiveBound, UmiRadRulerArchiveWrite, UmiRadRulerArchiveRead, UmiRadRulerArchiveValidate)
