/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/compensation_policy.h
 *
 * PURPOSE:
 *   Build reverse compensation order from completed saga steps while skipping non-compensatable actions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_COMPENSATION_POLICY_H
#define UMICOM_INTEGRATION_FABRIC_COMPENSATION_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/saga_plan.h"
#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricCompensationPolicy { bool stop_on_failure; size_t maximum_compensations; } UmiFabricCompensationPolicy;
void umi_fabric_compensation_policy_default(UmiFabricCompensationPolicy *policy);
UmiStatus umi_fabric_compensation_order(const UmiFabricSagaPlan *plan,size_t completed_steps,char out_step_ids[UMI_FABRIC_MAX_STEPS][UMI_FABRIC_ID_CAPACITY],size_t *out_count);

#ifdef __cplusplus
}
#endif
#endif
