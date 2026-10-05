/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/confirmation.c
 *
 * PURPOSE:
 *   Implement represent trade confirmation terms and confirmation state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/confirmation.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury confirmation from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_confirmation_init(UmiTreasuryConfirmation *value,
    const char *id,
    const char *trade_id,
    bool sent,
    bool acknowledged) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status=umi_treasury_id_copy(value->trade_id,sizeof value->trade_id,trade_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    value->sent=sent;
    value->acknowledged=acknowledged;
    return umi_treasury_confirmation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury confirmation satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_confirmation_valid(const UmiTreasuryConfirmation *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->trade_id, '\0', sizeof(value->trade_id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && umi_treasury_id_valid(value->trade_id) && (!value->acknowledged || value->sent));
}

/*
 * Provide the treasury confirmation complete operation used by this module and its client
 * applications.
 */
bool umi_treasury_confirmation_complete(const UmiTreasuryConfirmation *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (bool)0;
    return value->sent && value->acknowledged;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryConfirmationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa768f42cda3541d7);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryConfirmation *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryConfirmation *)0)->trade_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryConfirmationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryConfirmation *)0)->id) - 1U +
        8U + sizeof(((UmiTreasuryConfirmation *)0)->trade_id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryConfirmationArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryConfirmation *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->trade_id, sizeof(value->trade_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sent);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->acknowledged);
}
static void UmiTreasuryConfirmationArchiveRead(UmiArchiveReader *reader, UmiTreasuryConfirmation *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->trade_id, sizeof(value->trade_id));
    value->sent = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->acknowledged = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiTreasuryConfirmationArchiveValidate(const UmiTreasuryConfirmation *value)
{
    return umi_treasury_confirmation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_confirmation_archive_encode, umi_treasury_confirmation_archive_decode,
    UmiTreasuryConfirmation, UmiTreasuryConfirmationArchiveSchema, UmiTreasuryConfirmationArchiveBound, UmiTreasuryConfirmationArchiveWrite, UmiTreasuryConfirmationArchiveRead, UmiTreasuryConfirmationArchiveValidate)
