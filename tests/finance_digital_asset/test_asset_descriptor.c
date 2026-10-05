/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_asset_descriptor.c
 *
 * PURPOSE:
 *   Implement the test asset descriptor behavior for
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

#include "umicom/finance/digital_asset/asset_descriptor.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/digital_asset/asset_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDigitalAssetDescriptorTransferEqual(const UmiDigitalAssetDescriptor *a, const UmiDigitalAssetDescriptor *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->symbol, b->symbol) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        strcmp(a->network_id.value, b->network_id.value) == 0 &&
        a->decimals == b->decimals &&
        a->native_asset == b->native_asset &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDigitalAssetDescriptorTransferTails(UmiDigitalAssetDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->symbol) + 1U;
        memset(value->symbol + used, 0xa5, sizeof(value->symbol) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->network_id.value) + 1U;
        memset(value->network_id.value + used, 0xa5, sizeof(value->network_id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDigitalAssetDescriptorTransferMalformed(const UmiDigitalAssetDescriptor *sample)
{
    (void)sample;
    {
        UmiDigitalAssetDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_asset_descriptor_valid(&invalid)) ||
            umi_digital_asset_asset_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalAssetDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.symbol, 'x', sizeof(invalid.symbol));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_asset_descriptor_valid(&invalid)) ||
            umi_digital_asset_asset_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated symbol was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalAssetDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_asset_descriptor_valid(&invalid)) ||
            umi_digital_asset_asset_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalAssetDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.network_id.value, 'x', sizeof(invalid.network_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_asset_descriptor_valid(&invalid)) ||
            umi_digital_asset_asset_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated network_id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDigitalAssetDescriptorTransferCases, UmiDigitalAssetDescriptor,
    umi_digital_asset_asset_descriptor_archive_encode, umi_digital_asset_asset_descriptor_archive_decode,
    UmiDigitalAssetDescriptorTransferEqual, UmiDigitalAssetDescriptorTransferTails, UmiDigitalAssetDescriptorTransferMalformed)

int main(void)
{
    UmiDigitalAssetDescriptor value;
    CHECK(umi_digital_asset_asset_descriptor_init(&value, "ASSET-BTC", "BTC", "Bitcoin", "BTC", 8U, true) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_asset_descriptor_valid(&value));
    if (UmiDigitalAssetDescriptorTransferCases(&value) != 0) return 1;

    return 0;
}
