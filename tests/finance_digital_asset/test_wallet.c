/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_wallet.c
 *
 * PURPOSE:
 *   Implement the test wallet behavior for
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

#include "umicom/finance/digital_asset/wallet.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/digital_asset/wallet.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDigitalAssetWalletTransferEqual(const UmiDigitalAssetWallet *a, const UmiDigitalAssetWallet *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        strcmp(a->network_id.value, b->network_id.value) == 0 &&
        a->custodial == b->custodial &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDigitalAssetWalletTransferTails(UmiDigitalAssetWallet *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
    {
        size_t used = strlen(value->network_id.value) + 1U;
        memset(value->network_id.value + used, 0xa5, sizeof(value->network_id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDigitalAssetWalletTransferMalformed(const UmiDigitalAssetWallet *sample)
{
    (void)sample;
    {
        UmiDigitalAssetWallet invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_wallet_valid(&invalid)) ||
            umi_digital_asset_wallet_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalAssetWallet invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_wallet_valid(&invalid)) ||
            umi_digital_asset_wallet_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalAssetWallet invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.network_id.value, 'x', sizeof(invalid.network_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_wallet_valid(&invalid)) ||
            umi_digital_asset_wallet_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated network_id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDigitalAssetWalletTransferCases, UmiDigitalAssetWallet,
    umi_digital_asset_wallet_archive_encode, umi_digital_asset_wallet_archive_decode,
    UmiDigitalAssetWalletTransferEqual, UmiDigitalAssetWalletTransferTails, UmiDigitalAssetWalletTransferMalformed)

int main(void)
{
    UmiDigitalAssetWallet value;
    CHECK(umi_digital_asset_wallet_init(&value, "WALLET-1", "Operations", "BTC", true) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_wallet_valid(&value));
    if (UmiDigitalAssetWalletTransferCases(&value) != 0) return 1;

    return 0;
}
