/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_split_rule.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the split rule Integration Fabric capability.
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
#include "umicom/integration/fabric/split_rule.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/integration/fabric/split_rule.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFabricSplitRuleTransferEqual(const UmiFabricSplitRule *a, const UmiFabricSplitRule *b)
{
    return strcmp(a->rule_id, b->rule_id) == 0 &&
        strcmp(a->expression, b->expression) == 0 &&
        a->maximum_parts == b->maximum_parts &&
        a->discard_empty == b->discard_empty;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFabricSplitRuleTransferTails(UmiFabricSplitRule *value)
{
    (void)value;
    {
        size_t used = strlen(value->rule_id) + 1U;
        memset(value->rule_id + used, 0xa5, sizeof(value->rule_id) - used);
    }
    {
        size_t used = strlen(value->expression) + 1U;
        memset(value->expression + used, 0xa5, sizeof(value->expression) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFabricSplitRuleTransferMalformed(const UmiFabricSplitRule *sample)
{
    (void)sample;
    {
        UmiFabricSplitRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.rule_id, 'x', sizeof(invalid.rule_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_split_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_split_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated rule_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricSplitRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.expression, 'x', sizeof(invalid.expression));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_split_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_split_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated expression was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFabricSplitRuleTransferCases, UmiFabricSplitRule,
    umi_fabric_split_rule_archive_encode, umi_fabric_split_rule_archive_decode,
    UmiFabricSplitRuleTransferEqual, UmiFabricSplitRuleTransferTails, UmiFabricSplitRuleTransferMalformed)

int main(void) {
    UmiFabricSplitRule item;
    CHECK(umi_fabric_split_rule_init(&item,"s","items[*]",100U,true)==UMI_STATUS_OK);
    if (UmiFabricSplitRuleTransferCases(&item) != 0) return 1;

    CHECK(item.maximum_parts==100U);
    return 0;
}
