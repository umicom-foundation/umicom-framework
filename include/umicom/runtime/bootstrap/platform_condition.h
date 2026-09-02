/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/platform_condition.h
 *
 * PURPOSE:
 *   Evaluate platform identifiers for conditional runtime composition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_PLATFORM_CONDITION_H
#define UMICOM_RUNTIME_BOOTSTRAP_PLATFORM_CONDITION_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Provide the bootstrap platform condition match operation used by this module and its
 * client applications.
 */
bool umi_bootstrap_platform_condition_match(const char *platform_id,
                                            const char *expected_platform_id);

#ifdef __cplusplus
}
#endif

#endif
