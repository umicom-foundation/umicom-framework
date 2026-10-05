/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/row_height.c
 *
 * PURPOSE:
 *   Describe fixed or adaptive row-height constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/row_height.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise ui ent row height from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_ent_row_height_init(UmiUiEntRowHeight *value){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->preferred=0;value->minimum=0;value->maximum=0;value->automatic=0;value->preferred=28;value->minimum=18;value->maximum=128;return UMI_STATUS_OK;}
/* Check that ui ent row height satisfies its contract before another service relies on it. */
int umi_ui_ent_row_height_validate(const UmiUiEntRowHeight *value){return value!=NULL&&value->minimum>0&&value->maximum>=value->minimum&&value->preferred>=value->minimum&&value->preferred<=value->maximum;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiEntRowHeightArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x38855ad08af18766);

    return schema;
}
static size_t UmiUiEntRowHeightArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiEntRowHeightArchiveWrite(UmiArchiveWriter *writer, const UmiUiEntRowHeight *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->preferred);
    UmiArchiveWriteSigned(writer, (int64_t)value->minimum);
    UmiArchiveWriteSigned(writer, (int64_t)value->maximum);
    UmiArchiveWriteSigned(writer, (int64_t)value->automatic);
}
static void UmiUiEntRowHeightArchiveRead(UmiArchiveReader *reader, UmiUiEntRowHeight *value)
{
    value->preferred = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->minimum = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->maximum = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->automatic = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiUiEntRowHeightArchiveValidate(const UmiUiEntRowHeight *value)
{
    return umi_ui_ent_row_height_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_ent_row_height_archive_encode, umi_ui_ent_row_height_archive_decode,
    UmiUiEntRowHeight, UmiUiEntRowHeightArchiveSchema, UmiUiEntRowHeightArchiveBound, UmiUiEntRowHeightArchiveWrite, UmiUiEntRowHeightArchiveRead, UmiUiEntRowHeightArchiveValidate)
