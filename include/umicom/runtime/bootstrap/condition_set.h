/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/condition_set.h
 *
 * PURPOSE:
 *   Combine conditions using explicit ALL or ANY semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_CONDITION_SET_H
#define UMICOM_RUNTIME_BOOTSTRAP_CONDITION_SET_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


void umi_bootstrap_condition_set_init(UmiBootstrapConditionSet *set,
                                      UmiBootstrapConditionMode mode);
UmiStatus umi_bootstrap_condition_set_add(UmiBootstrapConditionSet *set,
                                          const UmiBootstrapCondition *condition);
bool umi_bootstrap_condition_set_reduce(const bool *results,
                                        size_t count,
                                        UmiBootstrapConditionMode mode);

#ifdef __cplusplus
}
#endif

#endif
