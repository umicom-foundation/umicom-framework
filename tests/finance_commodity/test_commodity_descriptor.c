/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_commodity_descriptor.c
 *
 * PURPOSE:
 *   Implement the test commodity descriptor behavior for
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

#include "umicom/finance/commodity/commodity_descriptor.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/commodity/commodity_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCommodityDescriptorTransferEqual(const UmiCommodityDescriptor *a, const UmiCommodityDescriptor *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        strcmp(a->code, b->code) == 0 &&
        a->kind == b->kind &&
        strcmp(a->settlement_currency.code, b->settlement_currency.code) == 0 &&
        a->physical_delivery == b->physical_delivery &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCommodityDescriptorTransferTails(UmiCommodityDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->code) + 1U;
        memset(value->code + used, 0xa5, sizeof(value->code) - used);
    }
    {
        size_t used = strlen(value->settlement_currency.code) + 1U;
        memset(value->settlement_currency.code + used, 0xa5, sizeof(value->settlement_currency.code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCommodityDescriptorTransferMalformed(const UmiCommodityDescriptor *sample)
{
    (void)sample;
    {
        UmiCommodityDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_commodity_descriptor_valid(&invalid)) ||
            umi_commodity_commodity_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_commodity_descriptor_valid(&invalid)) ||
            umi_commodity_commodity_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.code, 'x', sizeof(invalid.code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_commodity_descriptor_valid(&invalid)) ||
            umi_commodity_commodity_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated code was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.settlement_currency.code, 'x', sizeof(invalid.settlement_currency.code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_commodity_descriptor_valid(&invalid)) ||
            umi_commodity_commodity_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated settlement_currency.code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCommodityDescriptorTransferCases, UmiCommodityDescriptor,
    umi_commodity_commodity_descriptor_archive_encode, umi_commodity_commodity_descriptor_archive_decode,
    UmiCommodityDescriptorTransferEqual, UmiCommodityDescriptorTransferTails, UmiCommodityDescriptorTransferMalformed)

int main(void)
{
    UmiCommodityDescriptor value;
    UmiCurrency currency = {{'U','S','D','\0'}};
    CHECK(umi_commodity_commodity_descriptor_init(&value, "CMD-WTI", "West Texas Intermediate", "WTI", UMI_COMMODITY_KIND_ENERGY, &currency, true) == UMI_STATUS_OK);
    CHECK(umi_commodity_commodity_descriptor_valid(&value));
    if (UmiCommodityDescriptorTransferCases(&value) != 0) return 1;

    CHECK(value.physical_delivery);
    return 0;
}
