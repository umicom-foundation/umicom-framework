/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_settlement.c
 *
 * PURPOSE:
 *   Implement represent payment settlement reference, amount and final settlement evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_settlement.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise payments payment settlement from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_settlement_init(UmiPaymentsPaymentSettlement *value,
    const char *id,
    const char *payment_id,
    const char *settlement_reference,
    int64_t amount_minor,
    bool settled) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_id_assign(&value->payment_id,payment_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->settlement_reference,sizeof value->settlement_reference,settlement_reference);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->amount_minor=amount_minor;
    value->settled=settled;
    return umi_payments_payment_settlement_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment settlement satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payment_settlement_valid(const UmiPaymentsPaymentSettlement *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->payment_id.value, '\0', sizeof(value->payment_id.value)) == NULL) return 0;
    if (memchr(value->settlement_reference, '\0', sizeof(value->settlement_reference)) == NULL) return 0;

    return value!=NULL && (value->settlement_reference[0]!='\0' && value->amount_minor>0);
}

/*
 * Provide the payments payment settlement complete operation used by this module and its
 * client applications.
 */
bool umi_payments_payment_settlement_complete(const UmiPaymentsPaymentSettlement *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->settled;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPaymentsPaymentSettlementArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1af8badef4dbcee7);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentSettlement *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentSettlement *)0)->payment_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentSettlement *)0)->settlement_reference)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPaymentsPaymentSettlementArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPaymentsPaymentSettlement *)0)->id.value) - 1U +
        8U + sizeof(((UmiPaymentsPaymentSettlement *)0)->payment_id.value) - 1U +
        8U + sizeof(((UmiPaymentsPaymentSettlement *)0)->settlement_reference) - 1U +
        8U +
        8U;
}
static void UmiPaymentsPaymentSettlementArchiveWrite(UmiArchiveWriter *writer, const UmiPaymentsPaymentSettlement *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->payment_id.value, sizeof(value->payment_id.value));
    UmiArchiveWriteText(writer, value->settlement_reference, sizeof(value->settlement_reference));
    UmiArchiveWriteSigned(writer, (int64_t)value->amount_minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->settled);
}
static void UmiPaymentsPaymentSettlementArchiveRead(UmiArchiveReader *reader, UmiPaymentsPaymentSettlement *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->payment_id.value, sizeof(value->payment_id.value));
    UmiArchiveReadText(reader, value->settlement_reference, sizeof(value->settlement_reference));
    value->amount_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->settled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiPaymentsPaymentSettlementArchiveValidate(const UmiPaymentsPaymentSettlement *value)
{
    return umi_payments_payment_settlement_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_payments_payment_settlement_archive_encode, umi_payments_payment_settlement_archive_decode,
    UmiPaymentsPaymentSettlement, UmiPaymentsPaymentSettlementArchiveSchema, UmiPaymentsPaymentSettlementArchiveBound, UmiPaymentsPaymentSettlementArchiveWrite, UmiPaymentsPaymentSettlementArchiveRead, UmiPaymentsPaymentSettlementArchiveValidate)
