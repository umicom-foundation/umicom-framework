/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/validation_plan.h
 *
 * PURPOSE:
 *   Collect validation gates and determine whether required evidence is complete.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_VALIDATION_PLAN_H
#define UMICOM_DEVELOPER_HELIX_VALIDATION_PLAN_H
#include "umicom/developer/helix/validation_gate.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the helix validation plan data shared with callers of this public contract.
 */
typedef struct UmiHelixValidationPlan { UmiHelixValidationGate gates[UMI_HELIX_MAX_ITEMS]; bool completed[UMI_HELIX_MAX_ITEMS]; bool passed[UMI_HELIX_MAX_ITEMS]; size_t count; } UmiHelixValidationPlan;
/* Add a validation gate. */
UmiStatus umi_helix_validation_plan_add(UmiHelixValidationPlan *plan,const UmiHelixValidationGate *gate);
/* Record one gate result by index. */
UmiStatus umi_helix_validation_plan_record(UmiHelixValidationPlan *plan,size_t index,bool passed);
/* Report whether every blocking gate has completed and passed. */
bool umi_helix_validation_plan_ready(const UmiHelixValidationPlan *plan);
#ifdef __cplusplus
}
#endif
#endif
