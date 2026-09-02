/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/saga_plan.h
 *
 * PURPOSE:
 *   Maintain unique saga steps in execution order for deterministic reverse compensation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_SAGA_PLAN_H
#define UMICOM_INTEGRATION_FABRIC_SAGA_PLAN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/saga_step.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric saga plan data shared with callers of this public contract.
 */
typedef struct UmiFabricSagaPlan {
    UmiFabricSagaStep items[UMI_FABRIC_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiFabricSagaPlan;
/**
 * Initialise fabric saga plan from caller-provided values so later operations receive a
 * known state.
 */
void umi_fabric_saga_plan_init(UmiFabricSagaPlan *registry);
/**
 * Add fabric saga plan only after its inputs and available capacity have been checked.
 */
UmiStatus umi_fabric_saga_plan_add(UmiFabricSagaPlan *registry,const UmiFabricSagaStep *item);
/**
 * Find fabric saga plan while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_fabric_saga_plan_find(const UmiFabricSagaPlan *registry,const char *id,UmiFabricSagaStep *out_item);
/**
 * Return the number of records represented by fabric saga plan without changing their
 * state.
 */
size_t umi_fabric_saga_plan_count(const UmiFabricSagaPlan *registry);

#ifdef __cplusplus
}
#endif
#endif
