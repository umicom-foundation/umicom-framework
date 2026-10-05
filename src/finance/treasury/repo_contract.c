/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/repo_contract.c
 *
 * PURPOSE:
 *   Implement model repo cash principal, collateral value and repo rate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/repo_contract.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury repo contract from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_repo_contract_init(UmiTreasuryRepoContract *value,
    const char *id,
    int64_t cash_principal_minor,
    int64_t collateral_value_minor,
    uint32_t repo_rate_bps) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->cash_principal_minor=cash_principal_minor;
    value->collateral_value_minor=collateral_value_minor;
    value->repo_rate_bps=repo_rate_bps;
    return umi_treasury_repo_contract_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury repo contract satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_repo_contract_valid(const UmiTreasuryRepoContract *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->cash_principal_minor > 0 && value->collateral_value_minor > 0 && value->repo_rate_bps <= 10000U);
}

/*
 * Provide the treasury repo contract haircut minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_repo_contract_haircut_minor(const UmiTreasuryRepoContract *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->collateral_value_minor > value->cash_principal_minor ? value->collateral_value_minor - value->cash_principal_minor : 0;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryRepoContractArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf546521baef0beff);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryRepoContract *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryRepoContractArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryRepoContract *)0)->id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTreasuryRepoContractArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryRepoContract *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->cash_principal_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->collateral_value_minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->repo_rate_bps);
}
static void UmiTreasuryRepoContractArchiveRead(UmiArchiveReader *reader, UmiTreasuryRepoContract *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->cash_principal_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->collateral_value_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->repo_rate_bps = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiTreasuryRepoContractArchiveValidate(const UmiTreasuryRepoContract *value)
{
    return umi_treasury_repo_contract_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_repo_contract_archive_encode, umi_treasury_repo_contract_archive_decode,
    UmiTreasuryRepoContract, UmiTreasuryRepoContractArchiveSchema, UmiTreasuryRepoContractArchiveBound, UmiTreasuryRepoContractArchiveWrite, UmiTreasuryRepoContractArchiveRead, UmiTreasuryRepoContractArchiveValidate)
