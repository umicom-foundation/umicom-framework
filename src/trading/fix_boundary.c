/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/fix_boundary.c
 *
 * PURPOSE:
 *   Validate minimal FIX-style sequence and session identifiers without binding to a specific FIX engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of fix boundary. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/fix_boundary.h"
#include "../base/value_archive_internal.h"
/* Check that fix session info satisfies its contract before another service relies on it. */
int umi_fix_session_info_valid(const UmiFixSessionInfo *s){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (s == NULL) return 0;
    if (memchr(s->sender_comp_id, '\0', sizeof(s->sender_comp_id)) == NULL) return 0;
    if (memchr(s->target_comp_id, '\0', sizeof(s->target_comp_id)) == NULL) return 0;
return s!=NULL&&s->sender_comp_id[0]!='\0'&&s->target_comp_id[0]!='\0'&&s->next_out_sequence>0U&&s->next_in_sequence>0U;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFixSessionInfoArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x007c12e9832ae43c);
    schema = (schema ^ (uint64_t)sizeof(((UmiFixSessionInfo *)0)->sender_comp_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFixSessionInfo *)0)->target_comp_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFixSessionInfoArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFixSessionInfo *)0)->sender_comp_id) - 1U +
        8U + sizeof(((UmiFixSessionInfo *)0)->target_comp_id) - 1U +
        8U +
        8U;
}
static void UmiFixSessionInfoArchiveWrite(UmiArchiveWriter *writer, const UmiFixSessionInfo *value)
{
    UmiArchiveWriteText(writer, value->sender_comp_id, sizeof(value->sender_comp_id));
    UmiArchiveWriteText(writer, value->target_comp_id, sizeof(value->target_comp_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->next_out_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->next_in_sequence);
}
static void UmiFixSessionInfoArchiveRead(UmiArchiveReader *reader, UmiFixSessionInfo *value)
{
    UmiArchiveReadText(reader, value->sender_comp_id, sizeof(value->sender_comp_id));
    UmiArchiveReadText(reader, value->target_comp_id, sizeof(value->target_comp_id));
    value->next_out_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->next_in_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiFixSessionInfoArchiveValidate(const UmiFixSessionInfo *value)
{
    return umi_fix_session_info_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fix_session_info_archive_encode, umi_fix_session_info_archive_decode,
    UmiFixSessionInfo, UmiFixSessionInfoArchiveSchema, UmiFixSessionInfoArchiveBound, UmiFixSessionInfoArchiveWrite, UmiFixSessionInfoArchiveRead, UmiFixSessionInfoArchiveValidate)
