/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/stock_borrow_contract.h
 *
 * PURPOSE:
 *   Model stock borrow quantity, mark value and borrow fee.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_STOCK_BORROW_CONTRACT_H
#define UMICOM_FINANCE_TREASURY_STOCK_BORROW_CONTRACT_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury stock borrow contract data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryStockBorrowContract {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t quantity;
    int64_t mark_value_minor;
    uint32_t borrow_fee_bps;
} UmiTreasuryStockBorrowContract;
/**
 * Initialise treasury stock borrow contract from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_stock_borrow_contract_init(UmiTreasuryStockBorrowContract *value,
    const char *id,
    int64_t quantity,
    int64_t mark_value_minor,
    uint32_t borrow_fee_bps);
/**
 * Check that treasury stock borrow contract satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_stock_borrow_contract_valid(const UmiTreasuryStockBorrowContract *value);
/**
 * Provide the treasury stock borrow contract annual fee minor operation used by this
 * module and its client applications.
 */
int64_t umi_treasury_stock_borrow_contract_annual_fee_minor(const UmiTreasuryStockBorrowContract *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_stock_borrow_contract_archive_encode(const UmiTreasuryStockBorrowContract *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_stock_borrow_contract_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryStockBorrowContract *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTreasuryStockBorrowContract object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_treasury_stock_borrow_contract_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTreasuryStockBorrowContract *workspace, UmiTreasuryStockBorrowContract *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_treasury_stock_borrow_contract_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTreasuryStockBorrowContract *expected, UmiTreasuryStockBorrowContract *value);


#ifdef __cplusplus
}
#endif
#endif
