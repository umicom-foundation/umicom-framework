/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/saga_plan.h
 *
 * PURPOSE:
 *   Maintain unique saga steps in execution order for deterministic reverse compensation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiFabricSagaPlan {
    UmiFabricSagaStep items[UMI_FABRIC_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiFabricSagaPlan;
void umi_fabric_saga_plan_init(UmiFabricSagaPlan *registry);
UmiStatus umi_fabric_saga_plan_add(UmiFabricSagaPlan *registry,const UmiFabricSagaStep *item);
UmiStatus umi_fabric_saga_plan_find(const UmiFabricSagaPlan *registry,const char *id,UmiFabricSagaStep *out_item);
size_t umi_fabric_saga_plan_count(const UmiFabricSagaPlan *registry);

#ifdef __cplusplus
}
#endif
#endif
