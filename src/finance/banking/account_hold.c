/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/account_hold.c
 *
 * PURPOSE:
 *   Implement represent ring-fenced account funds and explicit release state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/account_hold.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise banking account hold from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_banking_account_hold_init(UmiBankingAccountHold *value,
    const char *id,
    const char *account_id,
    int64_t amount_minor,
    bool active) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiBankingAccountHold candidate = {0};
    UmiStatus rc=umi_banking_id_assign(&candidate.id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&candidate.account_id,account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    candidate.amount_minor=amount_minor;
    candidate.active=active;
    if (!umi_banking_account_hold_valid(&candidate)) return UMI_STATUS_INVALID_ARGUMENT;
    *value = candidate;
    return UMI_STATUS_OK;
}
/*
 * Check that banking account hold satisfies its contract before another service relies on
 * it.
 */
bool umi_banking_account_hold_valid(const UmiBankingAccountHold *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->account_id.value, '\0', sizeof(value->account_id.value)) == NULL) return 0;

    return value!=NULL && umi_financial_id_is_valid(&value->id) &&
        umi_financial_id_is_valid(&value->account_id) && value->amount_minor>0;
}

/*
 * Provide the banking account hold releasable operation used by this module and its client
 * applications.
 */
bool umi_banking_account_hold_releasable(const UmiBankingAccountHold *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return umi_banking_account_hold_valid(value) && value->active;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiBankingAccountHoldArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd3fd0f28513cead9);
    schema = (schema ^ (uint64_t)sizeof(((UmiBankingAccountHold *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiBankingAccountHold *)0)->account_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiBankingAccountHoldArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiBankingAccountHold *)0)->id.value) - 1U +
        8U + sizeof(((UmiBankingAccountHold *)0)->account_id.value) - 1U +
        8U +
        8U;
}
static void UmiBankingAccountHoldArchiveWrite(UmiArchiveWriter *writer, const UmiBankingAccountHold *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->account_id.value, sizeof(value->account_id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->amount_minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiBankingAccountHoldArchiveRead(UmiArchiveReader *reader, UmiBankingAccountHold *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->account_id.value, sizeof(value->account_id.value));
    value->amount_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiBankingAccountHoldArchiveValidate(const UmiBankingAccountHold *value)
{
    return umi_banking_account_hold_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_banking_account_hold_archive_encode, umi_banking_account_hold_archive_decode,
    UmiBankingAccountHold, UmiBankingAccountHoldArchiveSchema, UmiBankingAccountHoldArchiveBound, UmiBankingAccountHoldArchiveWrite, UmiBankingAccountHoldArchiveRead, UmiBankingAccountHoldArchiveValidate)
