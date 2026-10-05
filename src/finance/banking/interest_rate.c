/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/interest_rate.c
 *
 * PURPOSE:
 *   Implement represent annualised banking interest rates with an explicit day-count basis.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/interest_rate.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise banking interest rate from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_banking_interest_rate_init(UmiBankingInterestRate *value,
    const char *id,
    int32_t annual_rate_bps,
    uint32_t day_count_basis) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->annual_rate_bps=annual_rate_bps;
    value->day_count_basis=day_count_basis;
    return umi_banking_interest_rate_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that banking interest rate satisfies its contract before another service relies on
 * it.
 */
bool umi_banking_interest_rate_valid(const UmiBankingInterestRate *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;

    return value!=NULL && (value->annual_rate_bps>=-10000 && value->annual_rate_bps<=100000 && (value->day_count_basis==360U||value->day_count_basis==365U));
}

/*
 * Provide the banking interest rate rate bps operation used by this module and its client
 * applications.
 */
int32_t umi_banking_interest_rate_rate_bps(const UmiBankingInterestRate *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (int32_t)0;
    return value->annual_rate_bps;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiBankingInterestRateArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x204c660bdcba720f);
    schema = (schema ^ (uint64_t)sizeof(((UmiBankingInterestRate *)0)->id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiBankingInterestRateArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiBankingInterestRate *)0)->id.value) - 1U +
        8U +
        8U;
}
static void UmiBankingInterestRateArchiveWrite(UmiArchiveWriter *writer, const UmiBankingInterestRate *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->annual_rate_bps);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->day_count_basis);
}
static void UmiBankingInterestRateArchiveRead(UmiArchiveReader *reader, UmiBankingInterestRate *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    value->annual_rate_bps = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->day_count_basis = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiBankingInterestRateArchiveValidate(const UmiBankingInterestRate *value)
{
    return umi_banking_interest_rate_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_banking_interest_rate_archive_encode, umi_banking_interest_rate_archive_decode,
    UmiBankingInterestRate, UmiBankingInterestRateArchiveSchema, UmiBankingInterestRateArchiveBound, UmiBankingInterestRateArchiveWrite, UmiBankingInterestRateArchiveRead, UmiBankingInterestRateArchiveValidate)
