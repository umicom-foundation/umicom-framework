/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_party.c
 *
 * PURPOSE:
 *   Implement represent canonical debtor/creditor identity and account routing data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_party.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise payments payment party from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_party_init(UmiPaymentsPaymentParty *value,
    const char *id,
    const char *account_id,
    const char *display_name,
    const char *bank_code) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_id_assign(&value->account_id,account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->display_name,sizeof value->display_name,display_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    rc=umi_financial_core_copy(value->bank_code,sizeof value->bank_code,bank_code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    return umi_payments_payment_party_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment party satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_payment_party_valid(const UmiPaymentsPaymentParty *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->account_id.value, '\0', sizeof(value->account_id.value)) == NULL) return 0;
    if (memchr(value->display_name, '\0', sizeof(value->display_name)) == NULL) return 0;
    if (memchr(value->bank_code, '\0', sizeof(value->bank_code)) == NULL) return 0;

    return value!=NULL && (value->display_name[0]!='\0' && value->bank_code[0]!='\0');
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPaymentsPaymentPartyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5b32dab7472a2f65);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentParty *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentParty *)0)->account_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentParty *)0)->display_name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentParty *)0)->bank_code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPaymentsPaymentPartyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPaymentsPaymentParty *)0)->id.value) - 1U +
        8U + sizeof(((UmiPaymentsPaymentParty *)0)->account_id.value) - 1U +
        8U + sizeof(((UmiPaymentsPaymentParty *)0)->display_name) - 1U +
        8U + sizeof(((UmiPaymentsPaymentParty *)0)->bank_code) - 1U;
}
static void UmiPaymentsPaymentPartyArchiveWrite(UmiArchiveWriter *writer, const UmiPaymentsPaymentParty *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->account_id.value, sizeof(value->account_id.value));
    UmiArchiveWriteText(writer, value->display_name, sizeof(value->display_name));
    UmiArchiveWriteText(writer, value->bank_code, sizeof(value->bank_code));
}
static void UmiPaymentsPaymentPartyArchiveRead(UmiArchiveReader *reader, UmiPaymentsPaymentParty *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->account_id.value, sizeof(value->account_id.value));
    UmiArchiveReadText(reader, value->display_name, sizeof(value->display_name));
    UmiArchiveReadText(reader, value->bank_code, sizeof(value->bank_code));
}
static UmiStatus UmiPaymentsPaymentPartyArchiveValidate(const UmiPaymentsPaymentParty *value)
{
    return umi_payments_payment_party_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_payments_payment_party_archive_encode, umi_payments_payment_party_archive_decode,
    UmiPaymentsPaymentParty, UmiPaymentsPaymentPartyArchiveSchema, UmiPaymentsPaymentPartyArchiveBound, UmiPaymentsPaymentPartyArchiveWrite, UmiPaymentsPaymentPartyArchiveRead, UmiPaymentsPaymentPartyArchiveValidate)
