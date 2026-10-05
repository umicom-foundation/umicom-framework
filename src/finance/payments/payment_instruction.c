/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_instruction.c
 *
 * PURPOSE:
 *   Implement represent canonical payment economic meaning, parties, amount and lifecycle state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_instruction.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise payments payment instruction from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_instruction_init(UmiPaymentsPaymentInstruction *value,
    const char *id,
    const char *debtor_party_id,
    const char *creditor_party_id,
    const char *currency_code,
    int64_t amount_minor,
    UmiFinancialDate requested_date,
    UmiPaymentsStatus status,
    const char *idempotency_key) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_id_assign(&value->debtor_party_id,debtor_party_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_id_assign(&value->creditor_party_id,creditor_party_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_currency_from_code(currency_code,&value->currency);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->amount_minor=amount_minor;
    value->requested_date=requested_date;
    value->status=status;
    rc=umi_financial_core_copy(value->idempotency_key,sizeof value->idempotency_key,idempotency_key);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    return umi_payments_payment_instruction_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment instruction satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payment_instruction_valid(const UmiPaymentsPaymentInstruction *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->debtor_party_id.value, '\0', sizeof(value->debtor_party_id.value)) == NULL) return 0;
    if (memchr(value->creditor_party_id.value, '\0', sizeof(value->creditor_party_id.value)) == NULL) return 0;
    if (memchr(value->currency.code, '\0', sizeof(value->currency.code)) == NULL) return 0;
    if (memchr(value->idempotency_key, '\0', sizeof(value->idempotency_key)) == NULL) return 0;

    return value!=NULL && (value->amount_minor>0 && umi_financial_date_is_valid(value->requested_date) && value->status>=UMI_PAYMENTS_CREATED && value->status<=UMI_PAYMENTS_REJECTED && value->idempotency_key[0]!='\0');
}

/*
 * Provide the payments payment instruction final operation used by this module and its
 * client applications.
 */
bool umi_payments_payment_instruction_final(const UmiPaymentsPaymentInstruction *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->status==UMI_PAYMENTS_SETTLED || value->status==UMI_PAYMENTS_RETURNED || value->status==UMI_PAYMENTS_REJECTED;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPaymentsPaymentInstructionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x6869172005aba502);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentInstruction *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentInstruction *)0)->debtor_party_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentInstruction *)0)->creditor_party_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentInstruction *)0)->currency.code)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentInstruction *)0)->idempotency_key)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPaymentsPaymentInstructionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPaymentsPaymentInstruction *)0)->id.value) - 1U +
        8U + sizeof(((UmiPaymentsPaymentInstruction *)0)->debtor_party_id.value) - 1U +
        8U + sizeof(((UmiPaymentsPaymentInstruction *)0)->creditor_party_id.value) - 1U +
        8U + sizeof(((UmiPaymentsPaymentInstruction *)0)->currency.code) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U + sizeof(((UmiPaymentsPaymentInstruction *)0)->idempotency_key) - 1U;
}
static void UmiPaymentsPaymentInstructionArchiveWrite(UmiArchiveWriter *writer, const UmiPaymentsPaymentInstruction *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->debtor_party_id.value, sizeof(value->debtor_party_id.value));
    UmiArchiveWriteText(writer, value->creditor_party_id.value, sizeof(value->creditor_party_id.value));
    UmiArchiveWriteText(writer, value->currency.code, sizeof(value->currency.code));
    UmiArchiveWriteSigned(writer, (int64_t)value->amount_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->requested_date.year);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->requested_date.month);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->requested_date.day);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteText(writer, value->idempotency_key, sizeof(value->idempotency_key));
}
static void UmiPaymentsPaymentInstructionArchiveRead(UmiArchiveReader *reader, UmiPaymentsPaymentInstruction *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->debtor_party_id.value, sizeof(value->debtor_party_id.value));
    UmiArchiveReadText(reader, value->creditor_party_id.value, sizeof(value->creditor_party_id.value));
    UmiArchiveReadText(reader, value->currency.code, sizeof(value->currency.code));
    value->amount_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->requested_date.year = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->requested_date.month = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->requested_date.day = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->status = (UmiPaymentsStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->idempotency_key, sizeof(value->idempotency_key));
}
static UmiStatus UmiPaymentsPaymentInstructionArchiveValidate(const UmiPaymentsPaymentInstruction *value)
{
    return umi_payments_payment_instruction_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_payments_payment_instruction_archive_encode, umi_payments_payment_instruction_archive_decode,
    UmiPaymentsPaymentInstruction, UmiPaymentsPaymentInstructionArchiveSchema, UmiPaymentsPaymentInstructionArchiveBound, UmiPaymentsPaymentInstructionArchiveWrite, UmiPaymentsPaymentInstructionArchiveRead, UmiPaymentsPaymentInstructionArchiveValidate)
