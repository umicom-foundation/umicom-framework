/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/transform_rule.h
 *
 * PURPOSE:
 *   Describe a deterministic field/content transform without embedding a scripting runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_TRANSFORM_RULE_H
#define UMICOM_INTEGRATION_FABRIC_TRANSFORM_RULE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric transform rule data shared with callers of this public contract.
 */
typedef struct UmiFabricTransformRule {
    char rule_id[UMI_FABRIC_ID_CAPACITY];
    char source_path[UMI_FABRIC_TEXT_CAPACITY];
    char target_path[UMI_FABRIC_TEXT_CAPACITY];
    char operation[UMI_FABRIC_ID_CAPACITY];
    bool required;
} UmiFabricTransformRule;

/**
 * Initialise fabric transform rule from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_fabric_transform_rule_init(UmiFabricTransformRule *item, const char *rule_id, const char *source_path, const char *target_path, const char *operation, bool required);
/**
 * Check that fabric transform rule satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fabric_transform_rule_validate(const UmiFabricTransformRule *item);

#ifdef __cplusplus
}
#endif
#endif
