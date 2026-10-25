/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/settlement_netting.c
 *
 * PURPOSE:
 *   Implement calculate bilateral settlement netting across gross pay and receive legs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/conditional_archive_internal.h"
#include "umicom/finance/treasury/settlement_netting.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury settlement netting from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_settlement_netting_init(UmiTreasurySettlementNetting *value,
    const char *id,
    int64_t pay_minor,
    int64_t receive_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->pay_minor=pay_minor;
    value->receive_minor=receive_minor;
    return umi_treasury_settlement_netting_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury settlement netting satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_netting_valid(const UmiTreasurySettlementNetting *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->pay_minor >= 0 && value->receive_minor >= 0);
}

/*
 * Provide the treasury settlement netting net minor operation used by this module and its
 * client applications.
 */
/* The unchecked accessor is retained for review. The checked operation below
 * validates both nonnegative gross legs before subtraction, keeping malformed
 * restored/plugin values from causing signed overflow. Existing callers retain
 * their zero-on-NULL convention; other malformed input now also returns zero. */
#if 0
int64_t umi_treasury_settlement_netting_net_minor(const UmiTreasurySettlementNetting *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->receive_minor - value->pay_minor;
}

#endif

UmiStatus UmiTreasurySettlementNettingNetChecked(const UmiTreasurySettlementNetting *value,
    int64_t *out_minor)
{
    if (out_minor == NULL || !umi_treasury_settlement_netting_valid(value))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Each validated operand lies in [0, INT64_MAX], so their difference
     * lies in [-INT64_MAX, INT64_MAX] and is representable. */
    *out_minor = value->receive_minor - value->pay_minor;
    return UMI_STATUS_OK;
}

int64_t umi_treasury_settlement_netting_net_minor(const UmiTreasurySettlementNetting *value)
{
    int64_t result = 0;
    (void)UmiTreasurySettlementNettingNetChecked(value, &result);
    return result;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasurySettlementNettingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3941414b7809ccd8);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasurySettlementNetting *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasurySettlementNettingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasurySettlementNetting *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasurySettlementNettingArchiveWrite(UmiArchiveWriter *writer, const UmiTreasurySettlementNetting *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->pay_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->receive_minor);
}
static void UmiTreasurySettlementNettingArchiveRead(UmiArchiveReader *reader, UmiTreasurySettlementNetting *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->pay_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->receive_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasurySettlementNettingArchiveValidate(const UmiTreasurySettlementNetting *value)
{
    return umi_treasury_settlement_netting_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_settlement_netting_archive_encode, umi_treasury_settlement_netting_archive_decode,
    UmiTreasurySettlementNetting, UmiTreasurySettlementNettingArchiveSchema, UmiTreasurySettlementNettingArchiveBound, UmiTreasurySettlementNettingArchiveWrite, UmiTreasurySettlementNettingArchiveRead, UmiTreasurySettlementNettingArchiveValidate)

/* SDK callers choose staging storage independently of their thread stack.
 * Reuse this owner\'s exact schema, reader and validation before publishing
 * a destination; decoded values remain data, never execution authority. */
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_treasury_settlement_netting_archive_decode_with_workspace, UmiTreasurySettlementNetting,
    UmiTreasurySettlementNettingArchiveSchema, UmiTreasurySettlementNettingArchiveBound, UmiTreasurySettlementNettingArchiveRead, UmiTreasurySettlementNettingArchiveValidate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_treasury_settlement_netting_archive_decode_if_unchanged, umi_treasury_settlement_netting_archive_encode,
    UmiTreasurySettlementNetting, UmiTreasurySettlementNettingArchiveSchema, UmiTreasurySettlementNettingArchiveBound, UmiTreasurySettlementNettingArchiveRead, UmiTreasurySettlementNettingArchiveValidate)
