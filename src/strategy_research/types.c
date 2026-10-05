/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/types.c
 *
 * PURPOSE:
 *   Initialise and validate deterministic strategy research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/types.h"
#include "../base/value_archive_internal.h"

#include <string.h>

void umi_strategy_research_input_init(UmiStrategyResearchInput *input)
{
    if (input == NULL) return;
    (void)memset(input, 0, sizeof(*input));
    input->trusted = 1;
    input->deterministic = 1;
}

void umi_strategy_research_snapshot_init(UmiStrategyResearchSnapshot *snapshot)
{
    if (snapshot == NULL) return;
    (void)memset(snapshot, 0, sizeof(*snapshot));
}

UmiStatus umi_strategy_research_snapshot_validate(
    const UmiStrategyResearchSnapshot *snapshot)
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
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiStrategyResearchSnapshotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xee49e3d29e8a4931);
    schema = (schema ^ (uint64_t)sizeof(((UmiStrategyResearchSnapshot *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiStrategyResearchSnapshot *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiStrategyResearchSnapshotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiStrategyResearchSnapshot *)0)->id) - 1U +
        8U + sizeof(((UmiStrategyResearchSnapshot *)0)->label) - 1U +
        8U +
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
static void UmiStrategyResearchSnapshotArchiveWrite(UmiArchiveWriter *writer, const UmiStrategyResearchSnapshot *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteDouble(writer, value->value);
    UmiArchiveWriteDouble(writer, value->secondaryValue);
    UmiArchiveWriteDouble(writer, value->score);
    UmiArchiveWriteDouble(writer, value->ratio);
    UmiArchiveWriteDouble(writer, value->pnl);
    UmiArchiveWriteDouble(writer, value->risk);
    UmiArchiveWriteDouble(writer, value->delta);
    UmiArchiveWriteSigned(writer, (int64_t)value->ready);
    UmiArchiveWriteSigned(writer, (int64_t)value->attention);
    UmiArchiveWriteSigned(writer, (int64_t)value->blocked);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiStrategyResearchSnapshotArchiveRead(UmiArchiveReader *reader, UmiStrategyResearchSnapshot *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->value = UmiArchiveReadDouble(reader);
    value->secondaryValue = UmiArchiveReadDouble(reader);
    value->score = UmiArchiveReadDouble(reader);
    value->ratio = UmiArchiveReadDouble(reader);
    value->pnl = UmiArchiveReadDouble(reader);
    value->risk = UmiArchiveReadDouble(reader);
    value->delta = UmiArchiveReadDouble(reader);
    value->ready = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->attention = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->blocked = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiStrategyResearchSnapshotArchiveValidate(const UmiStrategyResearchSnapshot *value)
{
    return umi_strategy_research_snapshot_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_strategy_research_snapshot_archive_encode, umi_strategy_research_snapshot_archive_decode,
    UmiStrategyResearchSnapshot, UmiStrategyResearchSnapshotArchiveSchema, UmiStrategyResearchSnapshotArchiveBound, UmiStrategyResearchSnapshotArchiveWrite, UmiStrategyResearchSnapshotArchiveRead, UmiStrategyResearchSnapshotArchiveValidate)
