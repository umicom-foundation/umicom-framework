/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_transaction.c
 *
 * PURPOSE:
 *   Implement the test transaction behavior for
 *   Umicom Framework.
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
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/transaction.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/digital_asset/transaction.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDigitalAssetTransactionTransferEqual(const UmiDigitalAssetTransaction *a, const UmiDigitalAssetTransaction *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->network_id.value, b->network_id.value) == 0 &&
        strcmp(a->from_address, b->from_address) == 0 &&
        strcmp(a->to_address, b->to_address) == 0 &&
        a->amount.units == b->amount.units &&
        a->amount.scale == b->amount.scale &&
        strcmp(a->amount.asset_symbol, b->amount.asset_symbol) == 0 &&
        strcmp(a->transaction_hash, b->transaction_hash) == 0 &&
        a->state == b->state &&
        a->confirmations == b->confirmations;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDigitalAssetTransactionTransferTails(UmiDigitalAssetTransaction *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->network_id.value) + 1U;
        memset(value->network_id.value + used, 0xa5, sizeof(value->network_id.value) - used);
    }
    {
        size_t used = strlen(value->from_address) + 1U;
        memset(value->from_address + used, 0xa5, sizeof(value->from_address) - used);
    }
    {
        size_t used = strlen(value->to_address) + 1U;
        memset(value->to_address + used, 0xa5, sizeof(value->to_address) - used);
    }
    {
        size_t used = strlen(value->amount.asset_symbol) + 1U;
        memset(value->amount.asset_symbol + used, 0xa5, sizeof(value->amount.asset_symbol) - used);
    }
    {
        size_t used = strlen(value->transaction_hash) + 1U;
        memset(value->transaction_hash + used, 0xa5, sizeof(value->transaction_hash) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDigitalAssetTransactionTransferMalformed(const UmiDigitalAssetTransaction *sample)
{
    (void)sample;
    {
        UmiDigitalAssetTransaction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_transaction_valid(&invalid)) ||
            umi_digital_asset_transaction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalAssetTransaction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.network_id.value, 'x', sizeof(invalid.network_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_transaction_valid(&invalid)) ||
            umi_digital_asset_transaction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated network_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalAssetTransaction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.from_address, 'x', sizeof(invalid.from_address));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_transaction_valid(&invalid)) ||
            umi_digital_asset_transaction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated from_address was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalAssetTransaction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.to_address, 'x', sizeof(invalid.to_address));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_transaction_valid(&invalid)) ||
            umi_digital_asset_transaction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated to_address was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalAssetTransaction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.amount.asset_symbol, 'x', sizeof(invalid.amount.asset_symbol));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_transaction_valid(&invalid)) ||
            umi_digital_asset_transaction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated amount.asset_symbol was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalAssetTransaction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.transaction_hash, 'x', sizeof(invalid.transaction_hash));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_transaction_valid(&invalid)) ||
            umi_digital_asset_transaction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated transaction_hash was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDigitalAssetTransactionTransferCases, UmiDigitalAssetTransaction,
    umi_digital_asset_transaction_archive_encode, umi_digital_asset_transaction_archive_decode,
    UmiDigitalAssetTransactionTransferEqual, UmiDigitalAssetTransactionTransferTails, UmiDigitalAssetTransactionTransferMalformed)

int main(void)
{
    UmiDigitalAssetTransaction value;
    CHECK(umi_digital_asset_transaction_init(&value, "TX-1", "BTC", "from", "to", 1000, 8, "BTC") == UMI_STATUS_OK);
    CHECK(umi_digital_asset_transaction_valid(&value));
    if (UmiDigitalAssetTransactionTransferCases(&value) != 0) return 1;

    return 0;
}
