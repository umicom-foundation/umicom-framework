/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/frozen_columns.c
 *
 * PURPOSE:
 *   Describe leading and trailing frozen-column regions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/frozen_columns.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise ui ent frozen columns from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_ent_frozen_columns_init(UmiUiEntFrozenColumns *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->leading_count=0;value->trailing_count=0;value->total_columns=0;return UMI_STATUS_OK;}
/*
 * Check that ui ent frozen columns satisfies its contract before another service relies on
 * it.
 */
int umi_ui_ent_frozen_columns_validate(const UmiUiEntFrozenColumns *value){return value!=NULL&&value->leading_count+value->trailing_count<=value->total_columns;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiEntFrozenColumnsArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x64e37d1b62240a77);

    return schema;
}
static size_t UmiUiEntFrozenColumnsArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiUiEntFrozenColumnsArchiveWrite(UmiArchiveWriter *writer, const UmiUiEntFrozenColumns *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->leading_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->trailing_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->total_columns);
}
static void UmiUiEntFrozenColumnsArchiveRead(UmiArchiveReader *reader, UmiUiEntFrozenColumns *value)
{
    value->leading_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->trailing_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->total_columns = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
}
static UmiStatus UmiUiEntFrozenColumnsArchiveValidate(const UmiUiEntFrozenColumns *value)
{
    return umi_ui_ent_frozen_columns_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_ent_frozen_columns_archive_encode, umi_ui_ent_frozen_columns_archive_decode,
    UmiUiEntFrozenColumns, UmiUiEntFrozenColumnsArchiveSchema, UmiUiEntFrozenColumnsArchiveBound, UmiUiEntFrozenColumnsArchiveWrite, UmiUiEntFrozenColumnsArchiveRead, UmiUiEntFrozenColumnsArchiveValidate)
