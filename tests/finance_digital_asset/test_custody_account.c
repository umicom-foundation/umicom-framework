/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_custody_account.c
 *
 * PURPOSE:
 *   Implement the test custody account behavior for
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

#include "umicom/finance/digital_asset/custody_account.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/digital_asset/custody_account.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDigitalCustodyAccountTransferEqual(const UmiDigitalCustodyAccount *a, const UmiDigitalCustodyAccount *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->owner_party_id.value, b->owner_party_id.value) == 0 &&
        strcmp(a->wallet_id.value, b->wallet_id.value) == 0 &&
        a->segregated == b->segregated &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDigitalCustodyAccountTransferTails(UmiDigitalCustodyAccount *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->owner_party_id.value) + 1U;
        memset(value->owner_party_id.value + used, 0xa5, sizeof(value->owner_party_id.value) - used);
    }
    {
        size_t used = strlen(value->wallet_id.value) + 1U;
        memset(value->wallet_id.value + used, 0xa5, sizeof(value->wallet_id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDigitalCustodyAccountTransferMalformed(const UmiDigitalCustodyAccount *sample)
{
    (void)sample;
    {
        UmiDigitalCustodyAccount invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_custody_account_valid(&invalid)) ||
            umi_digital_asset_custody_account_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalCustodyAccount invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.owner_party_id.value, 'x', sizeof(invalid.owner_party_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_custody_account_valid(&invalid)) ||
            umi_digital_asset_custody_account_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated owner_party_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalCustodyAccount invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.wallet_id.value, 'x', sizeof(invalid.wallet_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_custody_account_valid(&invalid)) ||
            umi_digital_asset_custody_account_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated wallet_id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDigitalCustodyAccountTransferCases, UmiDigitalCustodyAccount,
    umi_digital_asset_custody_account_archive_encode, umi_digital_asset_custody_account_archive_decode,
    UmiDigitalCustodyAccountTransferEqual, UmiDigitalCustodyAccountTransferTails, UmiDigitalCustodyAccountTransferMalformed)

int main(void)
{
    UmiDigitalCustodyAccount value;
    UmiFinancialId owner = {{"PARTY-1"}};
    CHECK(umi_digital_asset_custody_account_init(&value, "CUST-1", &owner, "WALLET-1", true) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_custody_account_valid(&value));
    if (UmiDigitalCustodyAccountTransferCases(&value) != 0) return 1;

    CHECK(value.segregated);
    return 0;
}
