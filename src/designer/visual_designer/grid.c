/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/grid.c
 *
 * PURPOSE:
 *   Describe configurable design-time grid spacing and visibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/grid.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer grid from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_grid_init(UmiRadDesignGrid *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->spacing_x = 1;
    item->spacing_y = 1;
    item->visible = true;
    return UMI_STATUS_OK;
}
/* Check that visual designer grid satisfies its contract before another service relies on it. */
int umi_rad_grid_is_valid(const UmiRadDesignGrid *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->spacing_x > 0 && item->spacing_y > 0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadDesignGridArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x2e1edc25b4ddee05);

    return schema;
}
static size_t UmiRadDesignGridArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRadDesignGridArchiveWrite(UmiArchiveWriter *writer, const UmiRadDesignGrid *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->spacing_x);
    UmiArchiveWriteSigned(writer, (int64_t)value->spacing_y);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->visible);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->snap_enabled);
}
static void UmiRadDesignGridArchiveRead(UmiArchiveReader *reader, UmiRadDesignGrid *value)
{
    value->spacing_x = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->spacing_y = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->visible = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->snap_enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiRadDesignGridArchiveValidate(const UmiRadDesignGrid *value)
{
    return umi_rad_grid_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_grid_archive_encode, umi_rad_grid_archive_decode,
    UmiRadDesignGrid, UmiRadDesignGridArchiveSchema, UmiRadDesignGridArchiveBound, UmiRadDesignGridArchiveWrite, UmiRadDesignGridArchiveRead, UmiRadDesignGridArchiveValidate)
