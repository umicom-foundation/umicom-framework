/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/contract_book.h
 *
 * PURPOSE:
 *   Define a bounded book of physical commodity contracts.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_CONTRACT_BOOK_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_CONTRACT_BOOK_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"
#include "umicom/finance/commodity/physical_contract.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity contract book data shared with callers of this public contract.
 */
typedef struct UmiCommodityContractBook {
    UmiCommodityPhysicalContract items[UMI_COMMODITY_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiCommodityContractBook;

/* Reset a bounded catalogue before loading reference data. */
void umi_commodity_contract_book_init(UmiCommodityContractBook *catalogue);

/* Add a unique item and advance the deterministic catalogue revision. */
UmiStatus umi_commodity_contract_book_add(UmiCommodityContractBook *catalogue, const UmiCommodityPhysicalContract *item);

/* Resolve an item by its stable Framework identifier. */
const UmiCommodityPhysicalContract *umi_commodity_contract_book_find(const UmiCommodityContractBook *catalogue, const char *id);

#ifdef __cplusplus
}
#endif

#endif
