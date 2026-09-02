/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/route_rule.h
 *
 * PURPOSE:
 *   Describe ordered route matching by source, destination and message pattern.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_ROUTE_RULE_H
#define UMICOM_INTEGRATION_FABRIC_ROUTE_RULE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric route rule data shared with callers of this public contract.
 */
typedef struct UmiFabricRouteRule {
    char route_id[UMI_FABRIC_ID_CAPACITY];
    char source_pattern[UMI_FABRIC_TEXT_CAPACITY];
    char message_pattern[UMI_FABRIC_TEXT_CAPACITY];
    char destination_id[UMI_FABRIC_ID_CAPACITY];
    uint32_t priority;
    bool enabled;
} UmiFabricRouteRule;

/**
 * Initialise fabric route rule from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_fabric_route_rule_init(UmiFabricRouteRule *item, const char *route_id, const char *source_pattern, const char *message_pattern, const char *destination_id, uint32_t priority);
/**
 * Check that fabric route rule satisfies its contract before another service relies on it.
 */
UmiStatus umi_fabric_route_rule_validate(const UmiFabricRouteRule *item);

#ifdef __cplusplus
}
#endif
#endif
