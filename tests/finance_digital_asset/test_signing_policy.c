/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_signing_policy.c
 *
 * PURPOSE:
 *   Implement the test signing policy behavior for
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

#include "umicom/finance/digital_asset/signing_policy.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/digital_asset/signing_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDigitalSigningPolicyTransferEqual(const UmiDigitalSigningPolicy *a, const UmiDigitalSigningPolicy *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        a->required_approvals == b->required_approvals &&
        a->available_approvers == b->available_approvers &&
        a->hardware_required == b->hardware_required &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDigitalSigningPolicyTransferTails(UmiDigitalSigningPolicy *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDigitalSigningPolicyTransferMalformed(const UmiDigitalSigningPolicy *sample)
{
    (void)sample;
    {
        UmiDigitalSigningPolicy invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_signing_policy_valid(&invalid)) ||
            umi_digital_asset_signing_policy_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDigitalSigningPolicyTransferCases, UmiDigitalSigningPolicy,
    umi_digital_asset_signing_policy_archive_encode, umi_digital_asset_signing_policy_archive_decode,
    UmiDigitalSigningPolicyTransferEqual, UmiDigitalSigningPolicyTransferTails, UmiDigitalSigningPolicyTransferMalformed)

int main(void)
{
    UmiDigitalSigningPolicy value;
    CHECK(umi_digital_asset_signing_policy_init(&value, "POL-1", 2U, 3U, true) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_signing_policy_valid(&value));
    if (UmiDigitalSigningPolicyTransferCases(&value) != 0) return 1;

    return 0;
}
