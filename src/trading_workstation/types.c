/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/types.c
 *
 * PURPOSE:
 *   Initialise and validate shared professional trading-workstation evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/types.h"
#include "../base/value_archive_internal.h"

#include <string.h>

void umi_trading_professional_input_init(
    UmiTradingProfessionalInput *input)
{
    if (input == NULL) return;
    (void)memset(input, 0, sizeof(*input));
    input->trusted = 1;
}

void umi_trading_professional_snapshot_init(
    UmiTradingProfessionalSnapshot *snapshot)
{
    if (snapshot == NULL) return;
    (void)memset(snapshot, 0, sizeof(*snapshot));
}

UmiStatus umi_trading_professional_snapshot_validate(
    const UmiTradingProfessionalSnapshot *snapshot)
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
    if (snapshot->score < 0.0 || snapshot->score > 100.0 ||
        snapshot->ratio < -1000.0 || snapshot->ratio > 1000.0) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (snapshot->ready && snapshot->blocked) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTradingProfessionalSnapshotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf058f550078a0363);
    schema = (schema ^ (uint64_t)sizeof(((UmiTradingProfessionalSnapshot *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTradingProfessionalSnapshot *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTradingProfessionalSnapshotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTradingProfessionalSnapshot *)0)->id) - 1U +
        8U + sizeof(((UmiTradingProfessionalSnapshot *)0)->label) - 1U +
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
static void UmiTradingProfessionalSnapshotArchiveWrite(UmiArchiveWriter *writer, const UmiTradingProfessionalSnapshot *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteDouble(writer, value->value);
    UmiArchiveWriteDouble(writer, value->secondaryValue);
    UmiArchiveWriteDouble(writer, value->score);
    UmiArchiveWriteDouble(writer, value->ratio);
    UmiArchiveWriteDouble(writer, value->notional);
    UmiArchiveWriteDouble(writer, value->pnl);
    UmiArchiveWriteSigned(writer, (int64_t)value->ready);
    UmiArchiveWriteSigned(writer, (int64_t)value->attention);
    UmiArchiveWriteSigned(writer, (int64_t)value->blocked);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiTradingProfessionalSnapshotArchiveRead(UmiArchiveReader *reader, UmiTradingProfessionalSnapshot *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->value = UmiArchiveReadDouble(reader);
    value->secondaryValue = UmiArchiveReadDouble(reader);
    value->score = UmiArchiveReadDouble(reader);
    value->ratio = UmiArchiveReadDouble(reader);
    value->notional = UmiArchiveReadDouble(reader);
    value->pnl = UmiArchiveReadDouble(reader);
    value->ready = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->attention = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->blocked = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiTradingProfessionalSnapshotArchiveValidate(const UmiTradingProfessionalSnapshot *value)
{
    return umi_trading_professional_snapshot_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_trading_professional_snapshot_archive_encode, umi_trading_professional_snapshot_archive_decode,
    UmiTradingProfessionalSnapshot, UmiTradingProfessionalSnapshotArchiveSchema, UmiTradingProfessionalSnapshotArchiveBound, UmiTradingProfessionalSnapshotArchiveWrite, UmiTradingProfessionalSnapshotArchiveRead, UmiTradingProfessionalSnapshotArchiveValidate)
