/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_context_link_policy.c
 *
 * PURPOSE:
 *   Exercise context link policy behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/ui/mosaic/context_link_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/mosaic/context_link_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiMosaicContextLinkPolicyTransferEqual(const UmiUiMosaicContextLinkPolicy *a, const UmiUiMosaicContextLinkPolicy *b)
{
    return strcmp(a->group_id, b->group_id) == 0 &&
        strcmp(a->context_type, b->context_type) == 0 &&
        strcmp(a->member_id, b->member_id) == 0 &&
        a->colour_index == b->colour_index &&
        a->bidirectional == b->bidirectional;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiMosaicContextLinkPolicyTransferTails(UmiUiMosaicContextLinkPolicy *value)
{
    (void)value;
    {
        size_t used = strlen(value->group_id) + 1U;
        memset(value->group_id + used, 0xa5, sizeof(value->group_id) - used);
    }
    {
        size_t used = strlen(value->context_type) + 1U;
        memset(value->context_type + used, 0xa5, sizeof(value->context_type) - used);
    }
    {
        size_t used = strlen(value->member_id) + 1U;
        memset(value->member_id + used, 0xa5, sizeof(value->member_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiMosaicContextLinkPolicyTransferMalformed(const UmiUiMosaicContextLinkPolicy *sample)
{
    (void)sample;
    {
        UmiUiMosaicContextLinkPolicy invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_id, 'x', sizeof(invalid.group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_context_link_policy_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_context_link_policy_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiMosaicContextLinkPolicy invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.context_type, 'x', sizeof(invalid.context_type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_context_link_policy_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_context_link_policy_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated context_type was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiMosaicContextLinkPolicy invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.member_id, 'x', sizeof(invalid.member_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_context_link_policy_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_context_link_policy_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated member_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiMosaicContextLinkPolicyTransferCases, UmiUiMosaicContextLinkPolicy,
    umi_ui_mosaic_context_link_policy_archive_encode, umi_ui_mosaic_context_link_policy_archive_decode,
    UmiUiMosaicContextLinkPolicyTransferEqual, UmiUiMosaicContextLinkPolicyTransferTails, UmiUiMosaicContextLinkPolicyTransferMalformed)

int main(void) {
    UmiUiMosaicContextLinkPolicy value;
    umi_ui_mosaic_context_link_policy_init(&value);
    CHECK(umi_ui_mosaic_context_link_policy_set(&value, "link.green", "instrument", "trader.chart") == UMI_STATUS_OK);
    value.colour_index = 2U;
    CHECK(umi_ui_mosaic_context_link_policy_validate(&value) == UMI_STATUS_OK);
    if (UmiUiMosaicContextLinkPolicyTransferCases(&value) != 0) return 1;

    return 0;
}
