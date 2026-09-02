/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/condition_set.h
 *
 * PURPOSE:
 *   Combine conditions using explicit ALL or ANY semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_CONDITION_SET_H
#define UMICOM_RUNTIME_BOOTSTRAP_CONDITION_SET_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap condition set from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_condition_set_init(UmiBootstrapConditionSet *set,
                                      UmiBootstrapConditionMode mode);
/**
 * Add bootstrap condition set only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_condition_set_add(UmiBootstrapConditionSet *set,
                                          const UmiBootstrapCondition *condition);
/**
 * Provide the bootstrap condition set reduce operation used by this module and its client
 * applications.
 */
bool umi_bootstrap_condition_set_reduce(const bool *results,
                                        size_t count,
                                        UmiBootstrapConditionMode mode);

#ifdef __cplusplus
}
#endif

#endif
