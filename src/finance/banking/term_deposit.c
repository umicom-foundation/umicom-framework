/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/term_deposit.c
 *
 * PURPOSE:
 *   Implement represent principal, maturity and rollover intent for term deposits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/term_deposit.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise banking term deposit from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_banking_term_deposit_init(UmiBankingTermDeposit *value,
    const char *id,
    const char *customer_id,
    int64_t principal_minor,
    UmiFinancialDate start_date,
    UmiFinancialDate maturity_date,
    bool rollover) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->customer_id,customer_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->principal_minor=principal_minor;
    value->start_date=start_date;
    value->maturity_date=maturity_date;
    value->rollover=rollover;
    return umi_banking_term_deposit_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that banking term deposit satisfies its contract before another service relies on
 * it.
 */
bool umi_banking_term_deposit_valid(const UmiBankingTermDeposit *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->customer_id.value, '\0', sizeof(value->customer_id.value)) == NULL) return 0;

    return value!=NULL && (umi_financial_id_is_valid(&value->customer_id) && value->principal_minor>0 && umi_financial_date_is_valid(value->start_date) && umi_financial_date_is_valid(value->maturity_date) && umi_financial_date_compare(value->start_date,value->maturity_date)<0);
}

/*
 * Provide the banking term deposit auto rollover operation used by this module and its
 * client applications.
 */
bool umi_banking_term_deposit_auto_rollover(const UmiBankingTermDeposit *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->rollover;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiBankingTermDepositArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x0ceca14809a38acf);
    schema = (schema ^ (uint64_t)sizeof(((UmiBankingTermDeposit *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiBankingTermDeposit *)0)->customer_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiBankingTermDepositArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiBankingTermDeposit *)0)->id.value) - 1U +
        8U + sizeof(((UmiBankingTermDeposit *)0)->customer_id.value) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiBankingTermDepositArchiveWrite(UmiArchiveWriter *writer, const UmiBankingTermDeposit *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->customer_id.value, sizeof(value->customer_id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->principal_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->start_date.year);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->start_date.month);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->start_date.day);
    UmiArchiveWriteSigned(writer, (int64_t)value->maturity_date.year);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maturity_date.month);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maturity_date.day);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->rollover);
}
static void UmiBankingTermDepositArchiveRead(UmiArchiveReader *reader, UmiBankingTermDeposit *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->customer_id.value, sizeof(value->customer_id.value));
    value->principal_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->start_date.year = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->start_date.month = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->start_date.day = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->maturity_date.year = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->maturity_date.month = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->maturity_date.day = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->rollover = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiBankingTermDepositArchiveValidate(const UmiBankingTermDeposit *value)
{
    return umi_banking_term_deposit_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_banking_term_deposit_archive_encode, umi_banking_term_deposit_archive_decode,
    UmiBankingTermDeposit, UmiBankingTermDepositArchiveSchema, UmiBankingTermDepositArchiveBound, UmiBankingTermDepositArchiveWrite, UmiBankingTermDepositArchiveRead, UmiBankingTermDepositArchiveValidate)
