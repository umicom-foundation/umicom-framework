/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/nostro_account.c
 *
 * PURPOSE:
 *   Implement track nostro ledger, available and reserved cash amounts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/nostro_account.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury nostro account from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_nostro_account_init(UmiTreasuryNostroAccount *value,
    const char *id,
    int64_t ledger_minor,
    int64_t reserved_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->ledger_minor=ledger_minor;
    value->reserved_minor=reserved_minor;
    return umi_treasury_nostro_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury nostro account satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_nostro_account_valid(const UmiTreasuryNostroAccount *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->reserved_minor >= 0);
}

/*
 * Provide the treasury nostro account available minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_nostro_account_available_minor(const UmiTreasuryNostroAccount *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->ledger_minor - value->reserved_minor;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryNostroAccountArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb53ab30ee3fc4d06);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryNostroAccount *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryNostroAccountArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryNostroAccount *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryNostroAccountArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryNostroAccount *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->ledger_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->reserved_minor);
}
static void UmiTreasuryNostroAccountArchiveRead(UmiArchiveReader *reader, UmiTreasuryNostroAccount *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->ledger_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->reserved_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryNostroAccountArchiveValidate(const UmiTreasuryNostroAccount *value)
{
    return umi_treasury_nostro_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_nostro_account_archive_encode, umi_treasury_nostro_account_archive_decode,
    UmiTreasuryNostroAccount, UmiTreasuryNostroAccountArchiveSchema, UmiTreasuryNostroAccountArchiveBound, UmiTreasuryNostroAccountArchiveWrite, UmiTreasuryNostroAccountArchiveRead, UmiTreasuryNostroAccountArchiveValidate)
