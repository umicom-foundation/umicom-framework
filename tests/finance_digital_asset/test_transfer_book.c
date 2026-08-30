/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_transfer_book.c
 *
 * PURPOSE:
 *   Implement the test transfer book behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/transfer_book.h"

int main(void)
{
    UmiDigitalTransferBook catalogue;
    UmiDigitalTransferInstruction item;
    umi_digital_asset_transfer_book_init(&catalogue);
    CHECK(umi_digital_asset_transfer_instruction_init(&item, "XFER-1", "CUST-1", "bc1qdest", 500, 8, "BTC") == UMI_STATUS_OK);
    CHECK(umi_digital_asset_transfer_book_add(&catalogue, &item) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_transfer_book_add(&catalogue, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_digital_asset_transfer_book_find(&catalogue, "XFER-1") != NULL);
    CHECK(catalogue.revision == 1U);
    
    return 0;
}
