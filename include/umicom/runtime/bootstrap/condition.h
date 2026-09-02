/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/condition.h
 *
 * PURPOSE:
 *   Describe one property, feature, capability, platform or environment condition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_CONDITION_H
#define UMICOM_RUNTIME_BOOTSTRAP_CONDITION_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap condition from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_bootstrap_condition_init(UmiBootstrapCondition *condition,
                                       UmiBootstrapConditionKind kind,
                                       const char *key,
                                       const char *expected,
                                       bool negate);

#ifdef __cplusplus
}
#endif

#endif
