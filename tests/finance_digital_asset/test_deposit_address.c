/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_deposit_address.c
 *
 * PURPOSE:
 *   Implement the test deposit address behavior for
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

#include "umicom/finance/digital_asset/deposit_address.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/digital_asset/deposit_address.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDigitalDepositAddressTransferEqual(const UmiDigitalDepositAddress *a, const UmiDigitalDepositAddress *b)
{
    return strcmp(a->account_id.value, b->account_id.value) == 0 &&
        strcmp(a->asset_id.value, b->asset_id.value) == 0 &&
        strcmp(a->network_id.value, b->network_id.value) == 0 &&
        strcmp(a->address, b->address) == 0 &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDigitalDepositAddressTransferTails(UmiDigitalDepositAddress *value)
{
    (void)value;
    {
        size_t used = strlen(value->account_id.value) + 1U;
        memset(value->account_id.value + used, 0xa5, sizeof(value->account_id.value) - used);
    }
    {
        size_t used = strlen(value->asset_id.value) + 1U;
        memset(value->asset_id.value + used, 0xa5, sizeof(value->asset_id.value) - used);
    }
    {
        size_t used = strlen(value->network_id.value) + 1U;
        memset(value->network_id.value + used, 0xa5, sizeof(value->network_id.value) - used);
    }
    {
        size_t used = strlen(value->address) + 1U;
        memset(value->address + used, 0xa5, sizeof(value->address) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDigitalDepositAddressTransferMalformed(const UmiDigitalDepositAddress *sample)
{
    (void)sample;
    {
        UmiDigitalDepositAddress invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.account_id.value, 'x', sizeof(invalid.account_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_deposit_address_valid(&invalid)) ||
            umi_digital_asset_deposit_address_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated account_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalDepositAddress invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.asset_id.value, 'x', sizeof(invalid.asset_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_deposit_address_valid(&invalid)) ||
            umi_digital_asset_deposit_address_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated asset_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalDepositAddress invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.network_id.value, 'x', sizeof(invalid.network_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_deposit_address_valid(&invalid)) ||
            umi_digital_asset_deposit_address_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated network_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalDepositAddress invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.address, 'x', sizeof(invalid.address));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_deposit_address_valid(&invalid)) ||
            umi_digital_asset_deposit_address_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated address was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDigitalDepositAddressTransferCases, UmiDigitalDepositAddress,
    umi_digital_asset_deposit_address_archive_encode, umi_digital_asset_deposit_address_archive_decode,
    UmiDigitalDepositAddressTransferEqual, UmiDigitalDepositAddressTransferTails, UmiDigitalDepositAddressTransferMalformed)

int main(void)
{
    UmiDigitalDepositAddress value;
    CHECK(umi_digital_asset_deposit_address_init(&value, "CUST-1", "ASSET-BTC", "BTC", "bc1qdeposit") == UMI_STATUS_OK);
    CHECK(umi_digital_asset_deposit_address_valid(&value));
    if (UmiDigitalDepositAddressTransferCases(&value) != 0) return 1;

    return 0;
}
