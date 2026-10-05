/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/collateral_agreement.c
 *
 * PURPOSE:
 *   Implement model a collateral agreement threshold and minimum transfer amount.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/collateral_agreement.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury collateral agreement from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_collateral_agreement_init(UmiTreasuryCollateralAgreement *value,
    const char *id,
    int64_t threshold_minor,
    int64_t minimum_transfer_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->threshold_minor=threshold_minor;
    value->minimum_transfer_minor=minimum_transfer_minor;
    return umi_treasury_collateral_agreement_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury collateral agreement satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_collateral_agreement_valid(const UmiTreasuryCollateralAgreement *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->threshold_minor >= 0 && value->minimum_transfer_minor >= 0);
}

/*
 * Provide the treasury collateral agreement secured threshold minor operation used by this
 * module and its client applications.
 */
int64_t umi_treasury_collateral_agreement_secured_threshold_minor(const UmiTreasuryCollateralAgreement *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->threshold_minor + value->minimum_transfer_minor;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryCollateralAgreementArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x799613bf3f25630d);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryCollateralAgreement *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryCollateralAgreementArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryCollateralAgreement *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryCollateralAgreementArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryCollateralAgreement *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->threshold_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->minimum_transfer_minor);
}
static void UmiTreasuryCollateralAgreementArchiveRead(UmiArchiveReader *reader, UmiTreasuryCollateralAgreement *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->threshold_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->minimum_transfer_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryCollateralAgreementArchiveValidate(const UmiTreasuryCollateralAgreement *value)
{
    return umi_treasury_collateral_agreement_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_collateral_agreement_archive_encode, umi_treasury_collateral_agreement_archive_decode,
    UmiTreasuryCollateralAgreement, UmiTreasuryCollateralAgreementArchiveSchema, UmiTreasuryCollateralAgreementArchiveBound, UmiTreasuryCollateralAgreementArchiveWrite, UmiTreasuryCollateralAgreementArchiveRead, UmiTreasuryCollateralAgreementArchiveValidate)
