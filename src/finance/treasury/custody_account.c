/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/custody_account.c
 *
 * PURPOSE:
 *   Implement model a securities custody account and segregation status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/custody_account.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury custody account from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_custody_account_init(UmiTreasuryCustodyAccount *value,
    const char *id,
    const char *custodian_id,
    bool segregated) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status=umi_treasury_id_copy(value->custodian_id,sizeof value->custodian_id,custodian_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    value->segregated=segregated;
    return umi_treasury_custody_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury custody account satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_custody_account_valid(const UmiTreasuryCustodyAccount *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->custodian_id, '\0', sizeof(value->custodian_id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && umi_treasury_id_valid(value->custodian_id));
}

/*
 * Provide the treasury custody account is segregated operation used by this module and its
 * client applications.
 */
bool umi_treasury_custody_account_is_segregated(const UmiTreasuryCustodyAccount *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (bool)0;
    return value->segregated;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryCustodyAccountArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x22a56de9961f40c8);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryCustodyAccount *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryCustodyAccount *)0)->custodian_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryCustodyAccountArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryCustodyAccount *)0)->id) - 1U +
        8U + sizeof(((UmiTreasuryCustodyAccount *)0)->custodian_id) - 1U +
        8U;
}
static void UmiTreasuryCustodyAccountArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryCustodyAccount *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->custodian_id, sizeof(value->custodian_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->segregated);
}
static void UmiTreasuryCustodyAccountArchiveRead(UmiArchiveReader *reader, UmiTreasuryCustodyAccount *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->custodian_id, sizeof(value->custodian_id));
    value->segregated = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiTreasuryCustodyAccountArchiveValidate(const UmiTreasuryCustodyAccount *value)
{
    return umi_treasury_custody_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_custody_account_archive_encode, umi_treasury_custody_account_archive_decode,
    UmiTreasuryCustodyAccount, UmiTreasuryCustodyAccountArchiveSchema, UmiTreasuryCustodyAccountArchiveBound, UmiTreasuryCustodyAccountArchiveWrite, UmiTreasuryCustodyAccountArchiveRead, UmiTreasuryCustodyAccountArchiveValidate)
