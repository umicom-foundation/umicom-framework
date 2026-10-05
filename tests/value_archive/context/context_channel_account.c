/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_account.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/account.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/account.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAccountContextTransferEqual(const UmiAccountContext *a, const UmiAccountContext *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->account_id, b->account_id) == 0 &&
        strcmp(a->organisation_id, b->organisation_id) == 0 &&
        strcmp(a->book_id, b->book_id) == 0 &&
        strcmp(a->currency, b->currency) == 0 &&
        strcmp(a->account_type, b->account_type) == 0 &&
        strcmp(a->environment, b->environment) == 0 &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAccountContextTransferTails(UmiAccountContext *value)
{
    (void)value;
    {
        size_t used = strlen(value->account_id) + 1U;
        memset(value->account_id + used, 0xa5, sizeof(value->account_id) - used);
    }
    {
        size_t used = strlen(value->organisation_id) + 1U;
        memset(value->organisation_id + used, 0xa5, sizeof(value->organisation_id) - used);
    }
    {
        size_t used = strlen(value->book_id) + 1U;
        memset(value->book_id + used, 0xa5, sizeof(value->book_id) - used);
    }
    {
        size_t used = strlen(value->currency) + 1U;
        memset(value->currency + used, 0xa5, sizeof(value->currency) - used);
    }
    {
        size_t used = strlen(value->account_type) + 1U;
        memset(value->account_type + used, 0xa5, sizeof(value->account_type) - used);
    }
    {
        size_t used = strlen(value->environment) + 1U;
        memset(value->environment + used, 0xa5, sizeof(value->environment) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAccountContextTransferMalformed(const UmiAccountContext *sample)
{
    (void)sample;
    {
        UmiAccountContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.account_id, 'x', sizeof(invalid.account_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_account_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_account_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated account_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAccountContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.organisation_id, 'x', sizeof(invalid.organisation_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_account_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_account_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated organisation_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAccountContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.book_id, 'x', sizeof(invalid.book_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_account_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_account_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated book_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAccountContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.currency, 'x', sizeof(invalid.currency));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_account_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_account_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated currency was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAccountContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.account_type, 'x', sizeof(invalid.account_type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_account_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_account_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated account_type was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAccountContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.environment, 'x', sizeof(invalid.environment));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_account_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_account_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated environment was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAccountContextTransferCases, UmiAccountContext,
    umi_account_context_archive_encode, umi_account_context_archive_decode,
    UmiAccountContextTransferEqual, UmiAccountContextTransferTails, UmiAccountContextTransferMalformed)

int main(void)
{
    UmiAccountContext value;
    umi_account_context_init(&value);
    value.account_id[0] = 's';
    value.organisation_id[0] = 's';
    value.book_id[0] = 's';
    value.currency[0] = 's';
    value.account_type[0] = 's';
    value.environment[0] = 's';
    value.revision = (uint64_t)17U;
    if (umi_account_context_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiAccountContextTransferCases(&value) != 0) return 1;

    return 0;
}
