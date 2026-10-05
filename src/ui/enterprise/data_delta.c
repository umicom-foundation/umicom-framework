/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/data_delta.c
 *
 * PURPOSE:
 *   Implement data-delta validation and range tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/data_delta.h"
#include "../../base/value_archive_internal.h"
/* Check that ui ent data delta satisfies its contract before another service relies on it. */
int umi_ui_ent_data_delta_validate(const UmiUiEntDataDelta *d){return d&&d->kind>=UMI_UI_ENT_DELTA_INSERT&&d->kind<=UMI_UI_ENT_DELTA_RESET&&(d->kind==UMI_UI_ENT_DELTA_RESET||d->rows.count>0U);}
/*
 * Provide the ui ent data delta touches operation used by this module and its client
 * applications.
 */
int umi_ui_ent_data_delta_touches(const UmiUiEntDataDelta *d,size_t row){return umi_ui_ent_data_delta_validate(d)&&(d->kind==UMI_UI_ENT_DELTA_RESET||umi_ui_ent_span_contains(d->rows,row));}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiEntDataDeltaArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xda4ff76707d07bed);

    return schema;
}
static size_t UmiUiEntDataDeltaArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiEntDataDeltaArchiveWrite(UmiArchiveWriter *writer, const UmiUiEntDataDelta *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->rows.first);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->rows.count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
}
static void UmiUiEntDataDeltaArchiveRead(UmiArchiveReader *reader, UmiUiEntDataDelta *value)
{
    value->kind = (UmiUiEntDeltaKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->rows.first = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->rows.count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiUiEntDataDeltaArchiveValidate(const UmiUiEntDataDelta *value)
{
    return umi_ui_ent_data_delta_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_ent_data_delta_archive_encode, umi_ui_ent_data_delta_archive_decode,
    UmiUiEntDataDelta, UmiUiEntDataDeltaArchiveSchema, UmiUiEntDataDeltaArchiveBound, UmiUiEntDataDeltaArchiveWrite, UmiUiEntDataDeltaArchiveRead, UmiUiEntDataDeltaArchiveValidate)
