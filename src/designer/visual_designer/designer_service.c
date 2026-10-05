/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/designer_service.c
 *
 * PURPOSE:
 *   Aggregate visual designer readiness and active-session state for thin frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/designer_service.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer service from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_designer_service_init(UmiRadDesignerService *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->conformance_score = 100U;
    item->initialized = true;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer service satisfies its contract before another service relies on
 * it.
 */
int umi_rad_designer_service_is_valid(const UmiRadDesignerService *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->conformance_score <= 100U;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadDesignerServiceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x89e38b695f3274ba);

    return schema;
}
static size_t UmiRadDesignerServiceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadDesignerServiceArchiveWrite(UmiArchiveWriter *writer, const UmiRadDesignerService *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active_sessions);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->open_documents);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->conformance_score);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->initialized);
}
static void UmiRadDesignerServiceArchiveRead(UmiArchiveReader *reader, UmiRadDesignerService *value)
{
    value->active_sessions = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->open_documents = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->conformance_score = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->initialized = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadDesignerServiceArchiveValidate(const UmiRadDesignerService *value)
{
    return umi_rad_designer_service_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_designer_service_archive_encode, umi_rad_designer_service_archive_decode,
    UmiRadDesignerService, UmiRadDesignerServiceArchiveSchema, UmiRadDesignerServiceArchiveBound, UmiRadDesignerServiceArchiveWrite, UmiRadDesignerServiceArchiveRead, UmiRadDesignerServiceArchiveValidate)
