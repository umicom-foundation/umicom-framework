/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_transfer_instruction.c
 *
 * PURPOSE:
 *   Implement the test transfer instruction behavior for
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

#include "umicom/finance/digital_asset/transfer_instruction.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/digital_asset/transfer_instruction.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDigitalTransferInstructionTransferEqual(const UmiDigitalTransferInstruction *a, const UmiDigitalTransferInstruction *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->source_account_id.value, b->source_account_id.value) == 0 &&
        strcmp(a->destination_address, b->destination_address) == 0 &&
        a->amount.units == b->amount.units &&
        a->amount.scale == b->amount.scale &&
        strcmp(a->amount.asset_symbol, b->amount.asset_symbol) == 0 &&
        a->approved == b->approved &&
        a->submitted == b->submitted;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDigitalTransferInstructionTransferTails(UmiDigitalTransferInstruction *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->source_account_id.value) + 1U;
        memset(value->source_account_id.value + used, 0xa5, sizeof(value->source_account_id.value) - used);
    }
    {
        size_t used = strlen(value->destination_address) + 1U;
        memset(value->destination_address + used, 0xa5, sizeof(value->destination_address) - used);
    }
    {
        size_t used = strlen(value->amount.asset_symbol) + 1U;
        memset(value->amount.asset_symbol + used, 0xa5, sizeof(value->amount.asset_symbol) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDigitalTransferInstructionTransferMalformed(const UmiDigitalTransferInstruction *sample)
{
    (void)sample;
    {
        UmiDigitalTransferInstruction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_transfer_instruction_valid(&invalid)) ||
            umi_digital_asset_transfer_instruction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalTransferInstruction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_account_id.value, 'x', sizeof(invalid.source_account_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_transfer_instruction_valid(&invalid)) ||
            umi_digital_asset_transfer_instruction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_account_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalTransferInstruction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.destination_address, 'x', sizeof(invalid.destination_address));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_transfer_instruction_valid(&invalid)) ||
            umi_digital_asset_transfer_instruction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated destination_address was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalTransferInstruction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.amount.asset_symbol, 'x', sizeof(invalid.amount.asset_symbol));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_transfer_instruction_valid(&invalid)) ||
            umi_digital_asset_transfer_instruction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated amount.asset_symbol was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDigitalTransferInstructionTransferCases, UmiDigitalTransferInstruction,
    umi_digital_asset_transfer_instruction_archive_encode, umi_digital_asset_transfer_instruction_archive_decode,
    UmiDigitalTransferInstructionTransferEqual, UmiDigitalTransferInstructionTransferTails, UmiDigitalTransferInstructionTransferMalformed)

int main(void)
{
    UmiDigitalTransferInstruction value;
    CHECK(umi_digital_asset_transfer_instruction_init(&value, "XFER-1", "CUST-1", "bc1qdest", 500, 8, "BTC") == UMI_STATUS_OK);
    CHECK(umi_digital_asset_transfer_instruction_valid(&value));
    if (UmiDigitalTransferInstructionTransferCases(&value) != 0) return 1;

    return 0;
}
