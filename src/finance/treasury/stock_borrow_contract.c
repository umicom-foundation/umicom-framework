/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/stock_borrow_contract.c
 *
 * PURPOSE:
 *   Implement model stock borrow quantity, mark value and borrow fee.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/stock_borrow_contract.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury stock borrow contract from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_stock_borrow_contract_init(UmiTreasuryStockBorrowContract *value,
    const char *id,
    int64_t quantity,
    int64_t mark_value_minor,
    uint32_t borrow_fee_bps) {
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
    value->mark_value_minor=mark_value_minor;
    value->borrow_fee_bps=borrow_fee_bps;
    return umi_treasury_stock_borrow_contract_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury stock borrow contract satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_stock_borrow_contract_valid(const UmiTreasuryStockBorrowContract *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->quantity > 0 && value->mark_value_minor >= 0 && value->borrow_fee_bps <= 10000U);
}

/*
 * Provide the treasury stock borrow contract annual fee minor operation used by this
 * module and its client applications.
 */
int64_t umi_treasury_stock_borrow_contract_annual_fee_minor(const UmiTreasuryStockBorrowContract *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return (value->mark_value_minor * (int64_t)value->borrow_fee_bps) / 10000;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryStockBorrowContractArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x68874b6b5629ddca);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryStockBorrowContract *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryStockBorrowContractArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryStockBorrowContract *)0)->id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTreasuryStockBorrowContractArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryStockBorrowContract *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->quantity);
    UmiArchiveWriteSigned(writer, (int64_t)value->mark_value_minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->borrow_fee_bps);
}
static void UmiTreasuryStockBorrowContractArchiveRead(UmiArchiveReader *reader, UmiTreasuryStockBorrowContract *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->quantity = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->mark_value_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->borrow_fee_bps = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiTreasuryStockBorrowContractArchiveValidate(const UmiTreasuryStockBorrowContract *value)
{
    return umi_treasury_stock_borrow_contract_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_stock_borrow_contract_archive_encode, umi_treasury_stock_borrow_contract_archive_decode,
    UmiTreasuryStockBorrowContract, UmiTreasuryStockBorrowContractArchiveSchema, UmiTreasuryStockBorrowContractArchiveBound, UmiTreasuryStockBorrowContractArchiveWrite, UmiTreasuryStockBorrowContractArchiveRead, UmiTreasuryStockBorrowContractArchiveValidate)
