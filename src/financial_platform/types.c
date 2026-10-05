/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/types.c
 *
 * PURPOSE:
 *   Initialise and validate cross-domain shared financial platform evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/types.h"
#include "../base/value_archive_internal.h"

#include <string.h>

void umi_financial_platform_input_init(UmiFinancialPlatformInput *input)
{
    if (input == NULL) return;
    (void)memset(input, 0, sizeof(*input));
    input->trusted = 1;
}

void umi_financial_platform_snapshot_init(UmiFinancialPlatformSnapshot *snapshot)
{
    if (snapshot == NULL) return;
    (void)memset(snapshot, 0, sizeof(*snapshot));
}

UmiStatus umi_financial_platform_snapshot_validate(
    const UmiFinancialPlatformSnapshot *snapshot)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(snapshot->id, '\0', sizeof(snapshot->id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(snapshot->label, '\0', sizeof(snapshot->label)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    if (snapshot == NULL || snapshot->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (snapshot->score < 0.0 || snapshot->score > 100.0) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (snapshot->ready && snapshot->blocked) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (snapshot->approvalRequired && snapshot->ready) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFinancialPlatformSnapshotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd1415facf9e97eef);
    schema = (schema ^ (uint64_t)sizeof(((UmiFinancialPlatformSnapshot *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFinancialPlatformSnapshot *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFinancialPlatformSnapshotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFinancialPlatformSnapshot *)0)->id) - 1U +
        8U + sizeof(((UmiFinancialPlatformSnapshot *)0)->label) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiFinancialPlatformSnapshotArchiveWrite(UmiArchiveWriter *writer, const UmiFinancialPlatformSnapshot *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteDouble(writer, value->value);
    UmiArchiveWriteDouble(writer, value->secondaryValue);
    UmiArchiveWriteDouble(writer, value->score);
    UmiArchiveWriteDouble(writer, value->ratio);
    UmiArchiveWriteDouble(writer, value->delta);
    UmiArchiveWriteSigned(writer, (int64_t)value->ready);
    UmiArchiveWriteSigned(writer, (int64_t)value->attention);
    UmiArchiveWriteSigned(writer, (int64_t)value->blocked);
    UmiArchiveWriteSigned(writer, (int64_t)value->approvalRequired);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiFinancialPlatformSnapshotArchiveRead(UmiArchiveReader *reader, UmiFinancialPlatformSnapshot *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->value = UmiArchiveReadDouble(reader);
    value->secondaryValue = UmiArchiveReadDouble(reader);
    value->score = UmiArchiveReadDouble(reader);
    value->ratio = UmiArchiveReadDouble(reader);
    value->delta = UmiArchiveReadDouble(reader);
    value->ready = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->attention = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->blocked = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->approvalRequired = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiFinancialPlatformSnapshotArchiveValidate(const UmiFinancialPlatformSnapshot *value)
{
    return umi_financial_platform_snapshot_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_financial_platform_snapshot_archive_encode, umi_financial_platform_snapshot_archive_decode,
    UmiFinancialPlatformSnapshot, UmiFinancialPlatformSnapshotArchiveSchema, UmiFinancialPlatformSnapshotArchiveBound, UmiFinancialPlatformSnapshotArchiveWrite, UmiFinancialPlatformSnapshotArchiveRead, UmiFinancialPlatformSnapshotArchiveValidate)
