/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_transaction.c
 *
 * PURPOSE:
 *   Validate track atomic designer transactions and mutation counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/designer/visual_designer/transaction.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/transaction.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadDesignerTransactionTransferEqual(const UmiRadDesignerTransaction *a, const UmiRadDesignerTransaction *b)
{
    return strcmp(a->transaction_id, b->transaction_id) == 0 &&
        a->state == b->state &&
        a->mutation_count == b->mutation_count &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadDesignerTransactionTransferTails(UmiRadDesignerTransaction *value)
{
    (void)value;
    {
        size_t used = strlen(value->transaction_id) + 1U;
        memset(value->transaction_id + used, 0xa5, sizeof(value->transaction_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadDesignerTransactionTransferMalformed(const UmiRadDesignerTransaction *sample)
{
    (void)sample;
    {
        UmiRadDesignerTransaction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.transaction_id, 'x', sizeof(invalid.transaction_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_transaction_is_valid(&invalid)) ||
            umi_rad_transaction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated transaction_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadDesignerTransactionTransferCases, UmiRadDesignerTransaction,
    umi_rad_transaction_archive_encode, umi_rad_transaction_archive_decode,
    UmiRadDesignerTransactionTransferEqual, UmiRadDesignerTransactionTransferTails, UmiRadDesignerTransactionTransferMalformed)

int main(void){UmiRadDesignerTransaction item;CHECK(umi_rad_transaction_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_transaction_is_valid(&item));
    if (UmiRadDesignerTransactionTransferCases(&item) != 0) return 1;
return 0;}
