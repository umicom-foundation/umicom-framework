/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/securities_lending_contract.c
 *
 * PURPOSE:
 *   Implement model securities lending quantity, collateral value and fee rate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/securities_lending_contract.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury securities lending contract from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_securities_lending_contract_init(UmiTreasurySecuritiesLendingContract *value,
    const char *id,
    int64_t quantity,
    int64_t collateral_minor,
    uint32_t fee_bps) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->quantity=quantity;
    value->collateral_minor=collateral_minor;
    value->fee_bps=fee_bps;
    return umi_treasury_securities_lending_contract_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury securities lending contract satisfies its contract before another
 * service relies on it.
 */
bool umi_treasury_securities_lending_contract_valid(const UmiTreasurySecuritiesLendingContract *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->quantity > 0 && value->collateral_minor >= 0 && value->fee_bps <= 10000U);
}

/*
 * Provide the treasury securities lending contract annual fee minor operation used by this
 * module and its client applications.
 */
int64_t umi_treasury_securities_lending_contract_annual_fee_minor(const UmiTreasurySecuritiesLendingContract *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return (value->collateral_minor * (int64_t)value->fee_bps) / 10000;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasurySecuritiesLendingContractArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xeed6bd51eba6fb61);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasurySecuritiesLendingContract *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasurySecuritiesLendingContractArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasurySecuritiesLendingContract *)0)->id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTreasurySecuritiesLendingContractArchiveWrite(UmiArchiveWriter *writer, const UmiTreasurySecuritiesLendingContract *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->quantity);
    UmiArchiveWriteSigned(writer, (int64_t)value->collateral_minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->fee_bps);
}
static void UmiTreasurySecuritiesLendingContractArchiveRead(UmiArchiveReader *reader, UmiTreasurySecuritiesLendingContract *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->quantity = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->collateral_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->fee_bps = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiTreasurySecuritiesLendingContractArchiveValidate(const UmiTreasurySecuritiesLendingContract *value)
{
    return umi_treasury_securities_lending_contract_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_securities_lending_contract_archive_encode, umi_treasury_securities_lending_contract_archive_decode,
    UmiTreasurySecuritiesLendingContract, UmiTreasurySecuritiesLendingContractArchiveSchema, UmiTreasurySecuritiesLendingContractArchiveBound, UmiTreasurySecuritiesLendingContractArchiveWrite, UmiTreasurySecuritiesLendingContractArchiveRead, UmiTreasurySecuritiesLendingContractArchiveValidate)
