/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/data_snapshot.c
 *
 * PURPOSE:
 *   Describe an immutable logical data snapshot used by virtual views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/data_snapshot.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise ui ent data snapshot from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_ent_data_snapshot_init(UmiUiEntDataSnapshot *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->generation=0;value->row_count=0;value->column_count=0;value->complete=0;value->source_revision=0;value->complete=1;return UMI_STATUS_OK;}
/*
 * Check that ui ent data snapshot satisfies its contract before another service relies on
 * it.
 */
int umi_ui_ent_data_snapshot_validate(const UmiUiEntDataSnapshot *value){return value!=NULL&&value->column_count<=UMI_UI_ENT_MAX_COLUMNS;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUiEntDataSnapshotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb3319e6c80ecc6ce);

    return schema;
}
static size_t UmiUiEntDataSnapshotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUiEntDataSnapshotArchiveWrite(UmiArchiveWriter *writer, const UmiUiEntDataSnapshot *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->generation);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->row_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->column_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->complete);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->source_revision);
}
static void UmiUiEntDataSnapshotArchiveRead(UmiArchiveReader *reader, UmiUiEntDataSnapshot *value)
{
    value->generation = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->row_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->column_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->complete = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->source_revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiUiEntDataSnapshotArchiveValidate(const UmiUiEntDataSnapshot *value)
{
    return umi_ui_ent_data_snapshot_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ui_ent_data_snapshot_archive_encode, umi_ui_ent_data_snapshot_archive_decode,
    UmiUiEntDataSnapshot, UmiUiEntDataSnapshotArchiveSchema, UmiUiEntDataSnapshotArchiveBound, UmiUiEntDataSnapshotArchiveWrite, UmiUiEntDataSnapshotArchiveRead, UmiUiEntDataSnapshotArchiveValidate)
