/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_route_rule.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the route rule Integration Fabric capability.
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
#include "umicom/integration/fabric/route_rule.h"
#include "../value_archive/construction_cases.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/integration/fabric/route_rule.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFabricRouteRuleTransferEqual(const UmiFabricRouteRule *a, const UmiFabricRouteRule *b)
{
    return strcmp(a->route_id, b->route_id) == 0 &&
        strcmp(a->source_pattern, b->source_pattern) == 0 &&
        strcmp(a->message_pattern, b->message_pattern) == 0 &&
        strcmp(a->destination_id, b->destination_id) == 0 &&
        a->priority == b->priority &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFabricRouteRuleTransferTails(UmiFabricRouteRule *value)
{
    (void)value;
    {
        size_t used = strlen(value->route_id) + 1U;
        memset(value->route_id + used, 0xa5, sizeof(value->route_id) - used);
    }
    {
        size_t used = strlen(value->source_pattern) + 1U;
        memset(value->source_pattern + used, 0xa5, sizeof(value->source_pattern) - used);
    }
    {
        size_t used = strlen(value->message_pattern) + 1U;
        memset(value->message_pattern + used, 0xa5, sizeof(value->message_pattern) - used);
    }
    {
        size_t used = strlen(value->destination_id) + 1U;
        memset(value->destination_id + used, 0xa5, sizeof(value->destination_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFabricRouteRuleTransferMalformed(const UmiFabricRouteRule *sample)
{
    (void)sample;
    {
        UmiFabricRouteRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.route_id, 'x', sizeof(invalid.route_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_route_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_route_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated route_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricRouteRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_pattern, 'x', sizeof(invalid.source_pattern));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_route_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_route_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_pattern was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricRouteRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.message_pattern, 'x', sizeof(invalid.message_pattern));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_route_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_route_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated message_pattern was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricRouteRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.destination_id, 'x', sizeof(invalid.destination_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_route_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_route_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated destination_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFabricRouteRuleTransferCases, UmiFabricRouteRule,
    umi_fabric_route_rule_archive_encode, umi_fabric_route_rule_archive_decode,
    UmiFabricRouteRuleTransferEqual, UmiFabricRouteRuleTransferTails, UmiFabricRouteRuleTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiFabricRouteRuleTransferCasesWithWorkspace, UmiFabricRouteRule,
    umi_fabric_route_rule_archive_encode, umi_fabric_route_rule_archive_decode_with_workspace, UmiFabricRouteRuleTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiFabricRouteRuleConditionalArchiveCases, UmiFabricRouteRule,
    umi_fabric_route_rule_archive_encode, umi_fabric_route_rule_archive_decode_if_unchanged,
    UmiFabricRouteRuleTransferEqual, UmiFabricRouteRuleTransferTails, UmiFabricRouteRuleTransferMalformed)



/* A rejected replacement must preserve the previously accepted route rule value. */
static int UmiFabricRouteRuleConstructionCases(void)
{
    UmiFabricRouteRule *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_fabric_route_rule_init(value, "r1", "trader.*", "trade.*", "risk", 10U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->route_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_route_rule_init(value, oversized, "trader.*", "trade.*", "risk", 10U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_route_rule_init(value, value->route_id, "trader.*", "trade.*", "risk", 10U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->route_id, "r1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->source_pattern) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_route_rule_init(value, "r1", oversized, "trade.*", "risk", 10U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_route_rule_init(value, "r1", value->source_pattern, "trade.*", "risk", 10U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->source_pattern, "trader.*") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->message_pattern) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_route_rule_init(value, "r1", "trader.*", oversized, "risk", 10U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_route_rule_init(value, "r1", "trader.*", value->message_pattern, "risk", 10U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->message_pattern, "trade.*") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->destination_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_route_rule_init(value, "r1", "trader.*", "trade.*", oversized, 10U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_route_rule_init(value, "r1", "trader.*", "trade.*", value->destination_id, 10U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->destination_id, "risk") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiFabricRouteRuleConstructionCases() != 0) return 1;

    UmiFabricRouteRule item;
    CHECK(umi_fabric_route_rule_init(&item,"r1","trader.*","trade.*","risk",10U)==UMI_STATUS_OK);
    if (UmiFabricRouteRuleTransferCases(&item) != 0) return 1;
    if (UmiFabricRouteRuleTransferCasesWithWorkspace(&item) != 0) return 1;
    if (UmiFabricRouteRuleConditionalArchiveCases(&item) != 0) return 1;

    CHECK(item.enabled);
    return 0;
}
