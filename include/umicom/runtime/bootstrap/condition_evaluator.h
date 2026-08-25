/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/condition_evaluator.h
 *
 * PURPOSE:
 *   Evaluate auto-configuration conditions against Framework bootstrap context.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_CONDITION_EVALUATOR_H
#define UMICOM_RUNTIME_BOOTSTRAP_CONDITION_EVALUATOR_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


bool umi_bootstrap_condition_evaluate(
    const UmiBootstrapCondition *condition,
    const UmiBootstrapPropertySet *properties,
    const UmiBootstrapPropertySet *environment,
    const UmiBootstrapIdList *features,
    const UmiBootstrapIdList *capabilities,
    const char *platform_id);
bool umi_bootstrap_condition_set_evaluate(
    const UmiBootstrapConditionSet *set,
    const UmiBootstrapPropertySet *properties,
    const UmiBootstrapPropertySet *environment,
    const UmiBootstrapIdList *features,
    const UmiBootstrapIdList *capabilities,
    const char *platform_id);

#ifdef __cplusplus
}
#endif

#endif
