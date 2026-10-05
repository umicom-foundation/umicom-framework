/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_signing_request.c
 *
 * PURPOSE:
 *   Implement the test signing request behavior for
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

#include "umicom/finance/digital_asset/signing_request.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/digital_asset/signing_request.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDigitalSigningRequestTransferEqual(const UmiDigitalSigningRequest *a, const UmiDigitalSigningRequest *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->transaction_id.value, b->transaction_id.value) == 0 &&
        strcmp(a->policy_id.value, b->policy_id.value) == 0 &&
        a->required_approvals == b->required_approvals &&
        a->received_approvals == b->received_approvals &&
        a->closed == b->closed;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDigitalSigningRequestTransferTails(UmiDigitalSigningRequest *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->transaction_id.value) + 1U;
        memset(value->transaction_id.value + used, 0xa5, sizeof(value->transaction_id.value) - used);
    }
    {
        size_t used = strlen(value->policy_id.value) + 1U;
        memset(value->policy_id.value + used, 0xa5, sizeof(value->policy_id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDigitalSigningRequestTransferMalformed(const UmiDigitalSigningRequest *sample)
{
    (void)sample;
    {
        UmiDigitalSigningRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_signing_request_valid(&invalid)) ||
            umi_digital_asset_signing_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalSigningRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.transaction_id.value, 'x', sizeof(invalid.transaction_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_signing_request_valid(&invalid)) ||
            umi_digital_asset_signing_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated transaction_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalSigningRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.policy_id.value, 'x', sizeof(invalid.policy_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_signing_request_valid(&invalid)) ||
            umi_digital_asset_signing_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated policy_id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDigitalSigningRequestTransferCases, UmiDigitalSigningRequest,
    umi_digital_asset_signing_request_archive_encode, umi_digital_asset_signing_request_archive_decode,
    UmiDigitalSigningRequestTransferEqual, UmiDigitalSigningRequestTransferTails, UmiDigitalSigningRequestTransferMalformed)

int main(void)
{
    UmiDigitalSigningRequest value;
    CHECK(umi_digital_asset_signing_request_init(&value, "SIGN-1", "TX-1", "POL-1", 2U) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_signing_request_valid(&value));
    if (UmiDigitalSigningRequestTransferCases(&value) != 0) return 1;

    return 0;
}
