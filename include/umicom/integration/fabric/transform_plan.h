/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/transform_plan.h
 *
 * PURPOSE:
 *   Maintain an ordered set of unique transform rules before pipeline execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_TRANSFORM_PLAN_H
#define UMICOM_INTEGRATION_FABRIC_TRANSFORM_PLAN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/transform_rule.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric transform plan data shared with callers of this public contract.
 */
typedef struct UmiFabricTransformPlan {
    UmiFabricTransformRule items[UMI_FABRIC_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiFabricTransformPlan;
/**
 * Initialise fabric transform plan from caller-provided values so later operations receive
 * a known state.
 */
void umi_fabric_transform_plan_init(UmiFabricTransformPlan *registry);
/**
 * Add fabric transform plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_fabric_transform_plan_add(UmiFabricTransformPlan *registry,const UmiFabricTransformRule *item);
/**
 * Find fabric transform plan while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_fabric_transform_plan_find(const UmiFabricTransformPlan *registry,const char *id,UmiFabricTransformRule *out_item);
/**
 * Return the number of records represented by fabric transform plan without changing their
 * state.
 */
size_t umi_fabric_transform_plan_count(const UmiFabricTransformPlan *registry);

#ifdef __cplusplus
}
#endif
#endif
