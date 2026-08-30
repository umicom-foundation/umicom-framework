/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/environment_condition.h
 *
 * PURPOSE:
 *   Evaluate environment-style key/value overlays supplied to bootstrap.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_ENVIRONMENT_CONDITION_H
#define UMICOM_RUNTIME_BOOTSTRAP_ENVIRONMENT_CONDITION_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


bool umi_bootstrap_environment_condition_match(
    const UmiBootstrapPropertySet *environment,
    const char *key,
    const char *expected_value);

#ifdef __cplusplus
}
#endif

#endif
