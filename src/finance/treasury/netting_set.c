/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/netting_set.c
 *
 * PURPOSE:
 *   Implement define gross receivables and payables within a legally enforceable netting set.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/netting_set.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury netting set from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_netting_set_init(UmiTreasuryNettingSet *value,
    const char *id,
    int64_t receivable_minor,
    int64_t payable_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->receivable_minor=receivable_minor;
    value->payable_minor=payable_minor;
    return umi_treasury_netting_set_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury netting set satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_netting_set_valid(const UmiTreasuryNettingSet *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->receivable_minor >= 0 && value->payable_minor >= 0);
}

/*
 * Provide the treasury netting set net minor operation used by this module and its client
 * applications.
 */
int64_t umi_treasury_netting_set_net_minor(const UmiTreasuryNettingSet *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->receivable_minor - value->payable_minor;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryNettingSetArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x897abb31c7a97519);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryNettingSet *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryNettingSetArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryNettingSet *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryNettingSetArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryNettingSet *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->receivable_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->payable_minor);
}
static void UmiTreasuryNettingSetArchiveRead(UmiArchiveReader *reader, UmiTreasuryNettingSet *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->receivable_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->payable_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryNettingSetArchiveValidate(const UmiTreasuryNettingSet *value)
{
    return umi_treasury_netting_set_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_netting_set_archive_encode, umi_treasury_netting_set_archive_decode,
    UmiTreasuryNettingSet, UmiTreasuryNettingSetArchiveSchema, UmiTreasuryNettingSetArchiveBound, UmiTreasuryNettingSetArchiveWrite, UmiTreasuryNettingSetArchiveRead, UmiTreasuryNettingSetArchiveValidate)
