/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/aggregate_rule.h
 *
 * PURPOSE:
 *   Describe bounded correlation-based aggregation windows before the canonical messaging aggregator executes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_AGGREGATE_RULE_H
#define UMICOM_INTEGRATION_FABRIC_AGGREGATE_RULE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric aggregate rule data shared with callers of this public contract.
 */
typedef struct UmiFabricAggregateRule {
    char rule_id[UMI_FABRIC_ID_CAPACITY];
    char correlation_field[UMI_FABRIC_TEXT_CAPACITY];
    size_t expected_count;
    uint64_t timeout_ms;
    bool partial_allowed;
} UmiFabricAggregateRule;

/**
 * Initialise fabric aggregate rule from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_fabric_aggregate_rule_init(UmiFabricAggregateRule *item, const char *rule_id, const char *correlation_field, size_t expected_count, uint64_t timeout_ms, bool partial_allowed);
/**
 * Check that fabric aggregate rule satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fabric_aggregate_rule_validate(const UmiFabricAggregateRule *item);

#ifdef __cplusplus
}
#endif
#endif
