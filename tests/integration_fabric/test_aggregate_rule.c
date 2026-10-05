/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_aggregate_rule.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the aggregate rule Integration Fabric capability.
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
#include "umicom/integration/fabric/aggregate_rule.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/integration/fabric/aggregate_rule.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFabricAggregateRuleTransferEqual(const UmiFabricAggregateRule *a, const UmiFabricAggregateRule *b)
{
    return strcmp(a->rule_id, b->rule_id) == 0 &&
        strcmp(a->correlation_field, b->correlation_field) == 0 &&
        a->expected_count == b->expected_count &&
        a->timeout_ms == b->timeout_ms &&
        a->partial_allowed == b->partial_allowed;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFabricAggregateRuleTransferTails(UmiFabricAggregateRule *value)
{
    (void)value;
    {
        size_t used = strlen(value->rule_id) + 1U;
        memset(value->rule_id + used, 0xa5, sizeof(value->rule_id) - used);
    }
    {
        size_t used = strlen(value->correlation_field) + 1U;
        memset(value->correlation_field + used, 0xa5, sizeof(value->correlation_field) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFabricAggregateRuleTransferMalformed(const UmiFabricAggregateRule *sample)
{
    (void)sample;
    {
        UmiFabricAggregateRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.rule_id, 'x', sizeof(invalid.rule_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_aggregate_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_aggregate_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated rule_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricAggregateRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.correlation_field, 'x', sizeof(invalid.correlation_field));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_aggregate_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_aggregate_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated correlation_field was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFabricAggregateRuleTransferCases, UmiFabricAggregateRule,
    umi_fabric_aggregate_rule_archive_encode, umi_fabric_aggregate_rule_archive_decode,
    UmiFabricAggregateRuleTransferEqual, UmiFabricAggregateRuleTransferTails, UmiFabricAggregateRuleTransferMalformed)

int main(void) {
    UmiFabricAggregateRule item;
    CHECK(umi_fabric_aggregate_rule_init(&item,"a","order_id",4U,1000U,false)==UMI_STATUS_OK);
    if (UmiFabricAggregateRuleTransferCases(&item) != 0) return 1;

    CHECK(item.expected_count==4U);
    return 0;
}
