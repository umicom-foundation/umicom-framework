/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/ibkr_boundary.c
 *
 * PURPOSE:
 *   Validate Interactive Brokers connection settings without exposing vendor SDK types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of ibkr boundary. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/ibkr_boundary.h"
#include "../base/value_archive_internal.h"
/* Check that ibkr settings satisfies its contract before another service relies on it. */
int umi_ibkr_settings_valid(const UmiIbkrConnectionSettings *s){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (s == NULL) return 0;
    if (memchr(s->host, '\0', sizeof(s->host)) == NULL) return 0;
return s!=NULL&&s->host[0]!='\0'&&s->port>0U&&s->client_id>=0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiIbkrConnectionSettingsArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfcc8cf5cb74d8627);
    schema = (schema ^ (uint64_t)sizeof(((UmiIbkrConnectionSettings *)0)->host)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiIbkrConnectionSettingsArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiIbkrConnectionSettings *)0)->host) - 1U +
        8U +
        8U +
        8U;
}
static void UmiIbkrConnectionSettingsArchiveWrite(UmiArchiveWriter *writer, const UmiIbkrConnectionSettings *value)
{
    UmiArchiveWriteText(writer, value->host, sizeof(value->host));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->port);
    UmiArchiveWriteSigned(writer, (int64_t)value->client_id);
    UmiArchiveWriteSigned(writer, (int64_t)value->environment);
}
static void UmiIbkrConnectionSettingsArchiveRead(UmiArchiveReader *reader, UmiIbkrConnectionSettings *value)
{
    UmiArchiveReadText(reader, value->host, sizeof(value->host));
    value->port = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->client_id = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->environment = (UmiTradingEnvironment)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiIbkrConnectionSettingsArchiveValidate(const UmiIbkrConnectionSettings *value)
{
    return umi_ibkr_settings_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ibkr_settings_archive_encode, umi_ibkr_settings_archive_decode,
    UmiIbkrConnectionSettings, UmiIbkrConnectionSettingsArchiveSchema, UmiIbkrConnectionSettingsArchiveBound, UmiIbkrConnectionSettingsArchiveWrite, UmiIbkrConnectionSettingsArchiveRead, UmiIbkrConnectionSettingsArchiveValidate)
