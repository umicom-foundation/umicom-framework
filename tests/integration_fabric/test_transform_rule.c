/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_transform_rule.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the transform rule Integration Fabric capability.
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
#include "umicom/integration/fabric/transform_rule.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/integration/fabric/transform_rule.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFabricTransformRuleTransferEqual(const UmiFabricTransformRule *a, const UmiFabricTransformRule *b)
{
    return strcmp(a->rule_id, b->rule_id) == 0 &&
        strcmp(a->source_path, b->source_path) == 0 &&
        strcmp(a->target_path, b->target_path) == 0 &&
        strcmp(a->operation, b->operation) == 0 &&
        a->required == b->required;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFabricTransformRuleTransferTails(UmiFabricTransformRule *value)
{
    (void)value;
    {
        size_t used = strlen(value->rule_id) + 1U;
        memset(value->rule_id + used, 0xa5, sizeof(value->rule_id) - used);
    }
    {
        size_t used = strlen(value->source_path) + 1U;
        memset(value->source_path + used, 0xa5, sizeof(value->source_path) - used);
    }
    {
        size_t used = strlen(value->target_path) + 1U;
        memset(value->target_path + used, 0xa5, sizeof(value->target_path) - used);
    }
    {
        size_t used = strlen(value->operation) + 1U;
        memset(value->operation + used, 0xa5, sizeof(value->operation) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFabricTransformRuleTransferMalformed(const UmiFabricTransformRule *sample)
{
    (void)sample;
    {
        UmiFabricTransformRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.rule_id, 'x', sizeof(invalid.rule_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_transform_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_transform_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated rule_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricTransformRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_path, 'x', sizeof(invalid.source_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_transform_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_transform_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricTransformRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_path, 'x', sizeof(invalid.target_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_transform_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_transform_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricTransformRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.operation, 'x', sizeof(invalid.operation));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_transform_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_transform_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated operation was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFabricTransformRuleTransferCases, UmiFabricTransformRule,
    umi_fabric_transform_rule_archive_encode, umi_fabric_transform_rule_archive_decode,
    UmiFabricTransformRuleTransferEqual, UmiFabricTransformRuleTransferTails, UmiFabricTransformRuleTransferMalformed)

int main(void) {
    UmiFabricTransformRule item;
    CHECK(umi_fabric_transform_rule_init(&item,"t1","trade.price","risk.price","copy",true)==UMI_STATUS_OK);
    if (UmiFabricTransformRuleTransferCases(&item) != 0) return 1;

    CHECK(item.required);
    return 0;
}
