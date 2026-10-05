/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/replay_event.c
 *
 * PURPOSE:
 *   Validate deterministic replay event envelopes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of replay event. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/replay_event.h"
#include "../base/value_archive_internal.h"
/* Check that replay event satisfies its contract before another service relies on it. */
int umi_replay_event_valid(const UmiReplayEvent *e){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (e == NULL) return 0;
    if (memchr(e->type, '\0', sizeof(e->type)) == NULL) return 0;
    if (memchr(e->payload, '\0', sizeof(e->payload)) == NULL) return 0;
return e!=NULL&&e->sequence>0U&&e->event_time_ms>=0&&e->type[0]!='\0';}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiReplayEventArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x20a67ee274b1ab39);
    schema = (schema ^ (uint64_t)sizeof(((UmiReplayEvent *)0)->type)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiReplayEvent *)0)->payload)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiReplayEventArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U + sizeof(((UmiReplayEvent *)0)->type) - 1U +
        8U + sizeof(((UmiReplayEvent *)0)->payload) - 1U;
}
static void UmiReplayEventArchiveWrite(UmiArchiveWriter *writer, const UmiReplayEvent *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteSigned(writer, (int64_t)value->event_time_ms);
    UmiArchiveWriteText(writer, value->type, sizeof(value->type));
    UmiArchiveWriteText(writer, value->payload, sizeof(value->payload));
}
static void UmiReplayEventArchiveRead(UmiArchiveReader *reader, UmiReplayEvent *value)
{
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->event_time_ms = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    UmiArchiveReadText(reader, value->type, sizeof(value->type));
    UmiArchiveReadText(reader, value->payload, sizeof(value->payload));
}
static UmiStatus UmiReplayEventArchiveValidate(const UmiReplayEvent *value)
{
    return umi_replay_event_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_replay_event_archive_encode, umi_replay_event_archive_decode,
    UmiReplayEvent, UmiReplayEventArchiveSchema, UmiReplayEventArchiveBound, UmiReplayEventArchiveWrite, UmiReplayEventArchiveRead, UmiReplayEventArchiveValidate)
