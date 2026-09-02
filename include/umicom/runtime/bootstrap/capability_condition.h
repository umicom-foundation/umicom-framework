/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/capability_condition.h
 *
 * PURPOSE:
 *   Evaluate Framework runtime-capability presence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_CAPABILITY_CONDITION_H
#define UMICOM_RUNTIME_BOOTSTRAP_CAPABILITY_CONDITION_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Provide the bootstrap capability condition match operation used by this module and its
 * client applications.
 */
bool umi_bootstrap_capability_condition_match(
    const UmiBootstrapIdList *capabilities,
    const char *capability_id);

#ifdef __cplusplus
}
#endif

#endif
